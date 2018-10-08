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

/**
	\file Memtraits.h

	\brief Type traits for reading and writing to a buffer of memory.

	The functions defined here are helper functions used by a lot of Firestorm memory operations and
	are more or less reimplementations of the standard \c malloc, \c free, \c new, \c delete
	operations that you are used to using. Just like these operatios, however, none of these
	operations that are provided will check your work in any conceivable way. As such, it is up to you
	to build structures (they don't have to be uber complex, usually a simple Size/Capacity pair
	will do) to ensure the safety of these operations. For an example of this, see how it is 
	used in the ComponentBuffer class.

	It is possible to specialize these functions so that they will work with your own types, and in fact
	this sort of thing is more or less required for complex types like \c std::string and \c std::vector, and
	other non-primitive types that you would like to use with the system. Specializations of thhe functions
	for \c std::string are already defined.

	\warning I MEAN IT! EVEN IF YOU PROVIDE A SPECIALIZATION OF THIS CLASS FOR YOUR OWN CUSTOM
	TYPES, YOU SHOULD NOT, IN ANY WAY, PERFORM SAFETY CHECKS! THIS TRAITS CLASS AND ALL SPECIALIZATIONS OF IT ARE
	MEANT TO BE "AS THIN AS POSSIBLE" WRAPPERS AROUND TYPICAL MEMORY OPERATIONS.
**/

OPEN_NAMESPACE(Firestorm);

OPEN_NAMESPACE(Mem);

/**
	\brief Allocate a single new instance in the provided buffer and return the pointer.

	This function will perform an equivalent to a \c new operation using the pointer found at the index
	you provide through \c itemIndex as the memory block within which to allocate the stuff.

	\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
	\arg \c itemIndex The index into the buffer where the instance should be allocated.
	\arg \c args Arguments to pass into the constructor of the type.

	\warning THIS WILL OBLITERATE WHATEVER IS ALREADY AT THIS INDEX, SO BE CAREFUL!
**/
template<class T, class... Args>
static inline T* New(T* bufferHead,size_t itemIndex, Args&&... args)
{
	T* buffer = bufferHead + itemIndex;
	new(buffer) T(std::forward<Args>(args)...);
	return buffer;
}

template<class T>
static inline T* New(T* bufferHead, size_t itemIndex)
{
	return bufferHead + itemIndex;
}

template<>
static inline String* New(String* bufferHead, size_t itemIndex)
{
	String* buffer = bufferHead + itemIndex;
	new(buffer) String();
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
template<class T>
static inline std::pair<size_t,size_t> Delete(T* bufferHead, size_t itemIndex, size_t lastElement)
{
	std::swap(*(bufferHead +itemIndex), *(bufferHead + lastElement));
	return std::make_pair<size_t, size_t>(itemIndex, lastElement);
}

/**
	\brief Destroy all of the elements in the buffer while calling destructors if needed.

	This operation will delete all of the elements in the buffer and ensure that their destructors are called
	in the process (if required). This default implementation of the function for arithmetic types
	will simply memset the entire buffer to 0 since primitive arithmetic types don't require destructors.

	\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
	\arg \c itemIndex The index of the element that you want to delete.
	\arg \c lastElement The index of the last element of the buffer (typically \code{.cpp} BufferSize - 1 \endcode).
**/
template<class T>
static inline void Delete(T* bufferHead, size_t bufferSize)
{
	memset(bufferHead, 0, bufferSize);
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
template<>
static inline std::pair<size_t,size_t> Delete<String>(String* bufferHead, size_t itemIndex, size_t lastElement)
{
	String* item = bufferHead + itemIndex;
	item->~basic_string();
	std::swap(*item, *(bufferHead + lastElement));
	return std::make_pair(itemIndex, lastElement);
}


template<>
static inline void Delete<String>(String* bufferHead, size_t bufferSize)
{
	for(size_t i=0; i<bufferSize-1; ++i)
	{
		(bufferHead + i)->~basic_string();
	}
}



/**
	\brief Retrieve a reference to an item in the buffer.

	This operation will retrieve a reference to the item in the buffer at index \c itemIndex.

	\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
	\arg \c itemIndex The index of the element that you want to retrieve a reference to.

	\note The reference that is returned by all specializations of #Get should always be 
	non-const. This decision was made because it's easy to add const to a type (through a function that wraps 
	the operation perhaps?) however it's a bitch-and-a-half to remove CV Qualifiers from the type. Rather than
	imposing CV Qualification on the client of this class, I instead decided to allow the client to decide how 
	they want to CV qualify the return value since returning a non CV Qualified reference could be useful at times.

	\warning Ask me to change this at your peril. In English, don't ask. I'll say no. Fite me. 1v1 me bro.
**/
template<class T>
static inline T& Get(T* bufferHead, size_t itemIndex)
{
	return *(bufferHead + itemIndex);
}

template<class T>
static inline T* GetPtr(T* bufferHead, size_t itemIndex)
{
	return (bufferHead + itemIndex);
}

/**
	\brief Set the value of an element in the buffer.

	This operation will set the value of the item in the buffer at index \c itemIndex.

	\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
	\arg \c itemIndex The index of the element that you want to set the value of.
	\arg \c value The value to set the element at \c itemIndex to.
**/
template<class T>
static inline void Set(T* bufferHead, size_t itemIndex, const T& value)
{
	memcpy(bufferHead + itemIndex, &value, sizeof(T));
}

/**
	\brief Set the value of an element in the buffer with move semantics.

	This operation will set the value of the item in the buffer at index \c itemIndex using move semantics.

	\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
	\arg \c itemIndex The index of the element that you want to set the value of.
	\arg \c value The value to set the element at \c itemIndex to.
**/
template<class T>
static inline void Set(T* bufferHead, size_t itemIndex, T&& value)
{
	memmove(bufferHead + itemIndex, &value, sizeof(T));
}


/**
	\brief Set the value of an element in the buffer.

	This operation will set the value of the item in the buffer at index \c itemIndex.

	\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
	\arg \c itemIndex The index of the element that you want to set the value of.
	\arg \c value The value to set the element at \c itemIndex to.
**/
template<>
static inline void Set<String>(String* bufferHead, size_t itemIndex, const String& value)
{
	new(bufferHead + itemIndex) String(value);
}

/**
	\brief Set the value of an element in the buffer.

	This operation will set the value of the item in the buffer at index \c itemIndex.

	\arg \c bufferHead A pointer to the start of the buffer (I.E. the kind of pointer that malloc returns to you).
	\arg \c itemIndex The index of the element that you want to set the value of.
	\arg \c value The value to set the element at \c itemIndex to.
**/
template<>
static inline void Set<String>(String* bufferHead, size_t itemIndex, String&& value)
{
	new(bufferHead + itemIndex) String(std::move(value));
}

CLOSE_NAMESPACE(Mem);
CLOSE_NAMESPACE(Firestorm);

#endif
