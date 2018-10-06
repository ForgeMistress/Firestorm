///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Memtraits
//
//  Type trait stuff for the Component buffer thingy.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_MEMTRAITS_H_
#define LIBEXISTENCE_MEMTRAITS_H_
#pragma once

#include <libCore/TupleUtils.h>
#include <libCore/Assert.h>

#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

OPEN_NAMESPACE(Firestorm);

OPEN_NAMESPACE(Mem);
/**
	Allocate a memory block with enough space to contain the specified number of items.
**/
template<class T, typename type = std::decay<T>::value>
static inline type* Alloc(size_t numItems)
{
	return (type*)malloc(numItems * type_size);
}

/**
	Call memset on the specified buffer pointer.
**/
template<class T, typename type = std::decay<T>::value, std::size_t type_size = sizeof(type)>
static inline void Memset(type* buffer, int val)
{
	memset((void*)buffer, val, type_size);
}

/**
	Release the entire memory referenced by the specified buffer pointer.
**/
template<class T, typename type = std::decay<T>::value>
static inline void Free(type* buffer)
{
	free((void*)buffer);
}

/**
	\brief Type safety wrapper around memcpy.

	\arg \c to Where the data should be copied to.
	\arc \c from Where the data should be copied from.
	\return \c destination The return value of memcpy (which is "destination" so it's basically the \c to arg).
 **/
template<class T, typename type = std::decay<T>::value, std::size_t type_size = sizeof(type)>
static inline T* Memcopy(type* to, type* from)
{
	return (T*)memcpy((void*)to, (void*)from, type_size);
}

template<class T, typename type = std::decay<T>::value, std::size_t type_size = sizeof(type)>
static inline T* At(type* bufferHead, size_t index)
{
	return &bufferHead[index];
}

CLOSE_NAMESPACE(Mem);

OPEN_NAMESPACE(detail);

template<class T>
struct item_traits
{
	using type = typename std::decay<T>::type;
	static constexpr std::size_t type_size = sizeof(type);
};

CLOSE_NAMESPACE(detail);

#define traits_type(T) typename detail::item_traits<T>::type
/**
	\brief Type traits for reading and writing to a buffer of memory.

	This class is a helper class used by a lot of internal memory management functions and
	is more or less a reimplementation of the standard \c malloc, \c free, \c new, \c delete
	operations that you are used to using. Just like these operatios, however, none of these
	operations that are provided will check your work in any conceivable way. As such, it is up to you
	to build structures (they don't have to be uber complex, usually a simple Size/Capacity pair
	will do) to ensure the safety of these operations. For an example of this, see how it is 
	used in the ComponentBuffer class.

	This default Memtraits definition provides support for typical primitive types and as such, it cannot be
	used with complex types like std::string or std::vector. If support for these types is required, then
	it will be necessary to provide your own specialization of the Memtraits struct specifically tuned for 
	that particular type.

	\warning I MEAN IT! EVEN IF YOU PROVIDE A SPECIALIZATION OF THIS CLASS FOR YOUR OWN CUSTOM
	TYPES, YOU SHOULD NOT, IN ANY WAY, PERFORM SAFETY CHECKS! THIS TRAITS CLASS AND ALL SPECIALIZATIONS OF IT ARE
	MEANT TO BE "AS THIN AS POSSIBLE" WRAPPERS AROUND TYPICAL MEMORY OPERATIONS.
**/
template<class T>
struct Memtraits
{
	static_assert(std::is_arithmetic_v<detail::item_traits<T>::type>, 
		"the default specialization is for arithmetic types only"
	);
	/**
		\brief Allocate a single new instance in the provided buffer and return the pointer.

		This function will perform an equivalent to a \c new operation using the pointer found at the index
		you provide through \c itemIndex as the memory block within which to allocate the stuff.

		\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
		\arg \c itemIndex The index into the buffer where the instance should be allocated.
		\arg \c args Arguments to pass into the constructor of the type.

		\warning THIS WILL OBLITERATE WHATEVER IS ALREADY AT THIS INDEX, SO BE CAREFUL!
	**/
	template<class... Args>
	static inline traits_type(T)* New(
		traits_type(T)*    bufferHead, 
		         size_t    itemIndex,
		         Args&&... args)
	{
		traits_type(T)* buffer = &bufferHead[itemIndex];
		Mem::Memset<type>(buffer, std::forward<Args>(args)...);
		return buffer;
	}

	/**
		\brief Destroy an item held in the buffer. 
		
		Destroy an item held in the buffer. This will do so by swapping the data of the element
		at \c itemIndex with the data of the element at \c lastElement (which should, of couse, be the last
		element of the buffer, typically \code{.cpp} BufferSize - 1 \endcode).

		\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
		\arg \c itemIndex The index of the element that you want to delete.
		\arg \c lastElement The index of the last element of the buffer (typically \code{.cpp} BufferSize - 1 \endcode).
	**/
	static inline void Delete(
		traits_type(T)* bufferHead,
		         size_t itemIndex, 
		         size_t lastElement
	)
	{
		std::swap(bufferHead[itemIndex], bufferHead[lastElement]);
	}

	/**
		\brief Retrieve a reference to an item in the buffer.

		This operation will retrieve a reference to the item in the buffer at index \c itemIndex.

		\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
		\arg \c itemIndex The index of the element that you want to retrieve a reference to.

		\note The reference that is returned by all specializations of #Memtraits::Get should always be 
		non-const. This decision was made because it's easy to add const to a type (through a function that wraps 
		the operation perhaps?) however it's a bitch-and-a-half to remove CV Qualifiers from the type. Rather than
		imposing CV Qualification on the client of this class, I instead decided to allow the client to decide how 
		they want to CV qualify the return value since returning a non CV Qualified reference could be useful at times.

		\warning Ask me to change this at your peril. In English, don't ask. I'll say no. Fite me. 1v1 me bro.
	 **/
	static inline traits_type(T)& Get(
		traits_type(T)* bufferHead, 
		         size_t itemIndex
	)
	{
		return *buffer[itemIndex];
	}

	/**
		\brief Set the value of an element in the buffer.

		This operation will set the value of the item in the buffer at index \c itemIndex.

		\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
		\arg \c itemIndex The index of the element that you want to set the value of.
		\arg \c value The value to set the element at \c itemIndex to.
	 **/
	static inline void Set(
		      traits_type(T)* bufferHead, 
		               size_t itemIndex, 
		const traits_type(T)& value
	)
	{
		traits_type(T)* buffer = &bufferHead[itemIndex];
		Mem::Memcopy<type>(buffer, &value);
	}
};

template<>
struct Memtraits<String>
{
	/**
		\brief Allocate a single new \c String instance in the provided buffer and return the pointer.

		This function will perform an equivalent to a \c new operation using the pointer found at the index
		you provide through \c itemIndex as the memory block within which to allocate the stuff.

		\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
		\arg \c itemIndex The index into the buffer where the instance should be allocated.
		\arg \c args Arguments to pass into the constructor of the type.

		\warning THIS WILL OBLITERATE WHATEVER IS ALREADY AT THIS INDEX, SO BE CAREFUL!
	**/
	template<class... Args>
	static inline traits_type(String)* New(
		traits_type(String)*    bufferHead,
		              size_t    itemIndex,
		              Args&&... args
	)
	{
		type* buffer = &bufferHead[itemIndex];
		new(buffer) type(std::forward<Args>(args)...);
		return buffer;
	}

	/**
		\brief Destroy an item held in the buffer. 

		Destroy an item held in the buffer. This will do so by swapping the data of the element
		at itemIndex with the data of the element at lastElement (which should, of couse, be the last
		element of the buffer).

		Since this is the \c String specialization, this version will perform the same item/last swap as the other
		version, however this will also invoke the destructor of the \c String through ~basic_string, since C++ can be 
		dumb sometimes when it comes to type aliases. \c String is just an alias of std::string anyway, so we 
		good fam... unless you provide your own string type, in which case, you're on your own. Wanna deal with
		STL nonsense? That's your perogative. Just keep me the hell away from that low level STL stank.

		\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
		\arg \c itemIndex The index of the element that you want to delete.
		\arg \c lastElement The index of the last element of the buffer (typically Size - 1).
	**/
	static inline void Delete(
		traits_type(String)* bufferHead,
		              size_t itemIndex, 
		              size_t lastElement
	)
	{
		std::swap(bufferHead[itemIndex], bufferHead[lastElement]);
		bufferHead[lastElement].~basic_string();
	}

	/**
		\brief Retrieve a reference to \c String in the buffer.

		This operation will retrieve a reference to the \c String in the buffer at index \c itemIndex.

		\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
		\arg \c itemIndex The index of the element that you want to retrieve a reference to.
		\return \c value A non-const reference to the value stored at the specified index.

		\note The reference that is returned by all specializations of #Memtraits::Get should always be 
		non-const. This decision was made because it's easy to add const to a type (through a function that wraps 
		the operation perhaps?) however it's a bitch-and-a-half to remove CV Qualifiers from the type. Rather than
		imposing CV Qualification on the client of this class, I instead decided to allow the client to decide how 
		they want to CV qualify the return value since returning a non CV Qualified reference could be useful at times.

		\warning Ask me to change this at your peril. In English, don't ask. I'll say no. Fite me. 1v1 me bro.
	**/
	static inline typename detail::item_traits<String>::type& Get(
		traits_type(String)* bufferHead,
		              size_t itemIndex
	)
	{
		return bufferHead[itemIndex];
	}

	/**
		\brief Set the value of a \c String in the buffer.

		This operation will set the value of the \c String in the buffer at index \c itemIndex. The \c String
		specialization makes use of the \c clear(), \c reserve(), and \c assign() operations provided by
		the \c std::string type.

		\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
		\arg \c itemIndex The index of the element that you want to set the value of.
		\arg \c value The value to set the element at \c itemIndex to.
	**/
	static inline void Set(
		      traits_type(String)* bufferHead,
		                    size_t itemIndex, 
		const traits_type(String)& value
	)
	{
		detail::item_traits<String>::type& buffer = bufferHead[itemIndex];
		buffer.clear();
		buffer.reserve(value.size());
		buffer.assign(value);
	}
};

CLOSE_NAMESPACE(Firestorm);

#endif
