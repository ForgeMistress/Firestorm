///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SOA
//
//  A generalized implementation of a structure of arrays.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_SOA_H_
#define LIBCORE_SOA_H_
#pragma once

#include <libCore/libCore.h>
#include <libCore/Logger.h>
#include <EASTL/utility.h>
#include <EASTL/tuple.h>
#include <EASTL/bonus/tuple_vector.h>

OPEN_NAMESPACE(Firestorm);

#define FIRE_MAX_SOA_CAPACITY eastl::numeric_limits<size_t>::max()

template<class T> using raw = eastl::remove_pointer_t<eastl::remove_reference_t<T>>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <char... Digits>
constexpr size_t parse()
{
	// convert to array so we can use a loop instead of recursion
	char digits[] = {Digits...}; 

	// straightforward number parsing code
	auto result = 0u;
	for(auto c : digits)
	{
		result *= 10;
		result += c - '0';
	}

	return result;
}

template<size_t I>
struct soa_index {};

template<size_t I>
struct aos_index {};

#define FIRE_SOA_INDEX(SYMBOL, POS) static constexpr soa_index<POS> SYMBOL{};

template <char... Digits>
decltype(auto) operator"" _soa()
{
	return soa_index<parse<Digits...>()>{};
}

#ifdef FIRE_DISABLED
template<typename... Types>
static inline constexpr size_t CalculateSizeof()
{
	return (0 + ... + sizeof(Types));
}

template<class T>
static constexpr bool RequiresConstructorCall()
{
	if constexpr(eastl::is_arithmetic<T>::value)
	{
		return false;
	}
	else if constexpr(eastl::is_class<T>::value)
	{
		if constexpr(eastl::is_trivially_default_constructible<T>::value)
		{
			return false;
		}
	}
	// err on the side of caution.
	return true;
}

template<typename... Types>
static inline constexpr bool IsTriviallyConstructible()
{
	return (... && !RequiresConstructorCall<Types>());
}

template<class T>
static constexpr bool RequiresDestructorCall()
{
	if constexpr(eastl::is_arithmetic<T>::value)
	{
		return false;
	}
	else if constexpr(eastl::is_class<T>::value)
	{
		if constexpr(eastl::has_virtual_destructor<T>::value || !eastl::is_trivially_destructible<T>::value)
		{
			return true;
		}
	}
	// err on the side of caution.
	return true;
}

template<typename... Types>
static inline constexpr bool IsTriviallyDestructible()
{
	return (... && eastl::is_trivially_destructible<Types>::value);
}

template<typename... Types>
static inline constexpr bool AnyAreArrays()
{
	return (... && eastl::is_array<Types>::value);
}
#endif

template<class... Ts>
class SOA : public eastl::TupleVecInternal::TupleVecImpl<EASTLAllocatorType, make_index_sequence<sizeof...(Ts)>, Ts...>
{
public:
	template<size_t I>
	const eastl::TupleVecInternal::tuplevec_element_t<I, Ts...>* operator[](soa_index<I>)const
	{
		typedef eastl::TupleVecInternal::tuplevec_element_t<I, Ts...> Element;
		return eastl::TupleVecInternal::TupleVecLeaf<I, Element>::mpData;
	}
	template<size_t I>
	eastl::TupleVecInternal::tuplevec_element_t<I, Ts...>* operator[](soa_index<I>)
	{
		typedef eastl::TupleVecInternal::tuplevec_element_t<I, Ts...> Element;
		return eastl::TupleVecInternal::TupleVecLeaf<I, Element>::mpData;
	}
};

#ifdef FIRE_DISABLED
template<class... ItemsT>
struct SOA
{
	static_assert(!AnyAreArrays<ItemsT...>(), "can not store arrays within this structure (yet)");

	/**
		\brief Indexing helper for read (const) operations.
	 **/
	template<class T>
	struct SOARead
	{
		using type = raw<T>;
	public:
		SOARead(type* ptr, size_t size) : _ptr(ptr), _size(size) {}

		inline const type& operator[](const size_t& index) const
		{
			FIRE_ASSERT_MSG(index < _size, 
				Format("index '%d' exceeds the bounds of the buffer (size = %d)", index, _size));
			return _ptr[index];
		}
		inline size_t Size() const
		{
			return _size;
		}
	private:
		size_t _size;
		type* _ptr;
	};

	/**
		\brief Indexing helper for write (non-const) operations.
	**/
	template<class T>
	struct SOAWrite
	{
		using type = raw<T>;
	public:
		SOAWrite(type* ptr, size_t size) : _ptr(ptr), _size(size) {}

		inline type& operator[](const size_t& index)
		{
			FIRE_ASSERT_MSG(index < _size, 
				Format("index '%d' exceeds the bounds of the buffer (size = %d)", index, _size));
			return _ptr[index];
		}
		inline size_t Size() const
		{
			return _size;
		}
	private:
		size_t _size;
		type* _ptr;
	};


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Usings and constexprs

	/**
		Alias to the type of the tuple that this SOA uses to do its job.
	 **/
	using tuple_type = eastl::tuple<eastl::add_pointer_t<ItemsT>...>;

	/**
		Helper that retrieves the to the item at the tuple index \c I.
	**/
	template<size_t I> 
	using EElemT = eastl::tuple_element_t<I, tuple_type>;

	/**
		Helper that stores the number of items passed to the class template.
	 **/
	static constexpr size_t LLength = sizeof...(ItemsT); // number of individual members.

	/**
		The sum total of the size of all of the items stored within the container.
	 **/
	static constexpr size_t Sizeof = CalculateSizeof<ItemsT...>();

	/**
		Helper denoting whether or not all of the items in the container are trivially constructible.
	 **/
	static constexpr bool IsTrivialCons = IsTriviallyConstructible<ItemsT...>();

	/**
		Helper denoting whether or not all of the items in the container are trivially destructible.
	**/
	static constexpr bool IsTrivialDest = IsTriviallyDestructible<ItemsT...>();

private:
	size_t _size{ 0 };
	size_t _capacity{ 0 };
	size_t _sizeBytes{ 0 };
	void* _buffer{ nullptr };
	tuple_type _tuple;

public:
	/**
		\brief Retrieve an indexible handle for reading.

		This operator returns an SOARead that will allow you to safely index items in the buffer.
		SOARead is, as expected, read only, and will respect the constness of the calling scope.

		\code{.cpp}
		SOA<String, float, double> s;
		// ... initialize your items.
		size_t someIndex = value; // ... retrieved from somewhere.
		const String & readOnlyString = s[0_soa][someIndex];
		\endcode

		\warning This will assert if you attempt to index out of the range of the buffer.
	 **/
	template<
		size_t I,
		typename ElemT = EElemT<I>
	>
	const SOARead<ElemT> operator[](soa_index<I>) const
	{
		return SOARead<ElemT>{eastl::get<I>(_tuple), _size};
	}

	/**
		\brief Retrieve an indexible handle for writing.

		This operator returns an SOAWrite that will allow you to safely index items in the buffer.
		SOAWrite is, as expected, write only.

		\code{.cpp}
		SOA<String, float, double> s;
		// ... initialize your items.
		size_t someIndex = value; // ... retrieved from somewhere.
		s[0_soa][someIndex] = "I'm beginnin' to feel like a rap god (rap god).";
		\endcode

		\warning This will assert if you attempt to index out of the range of the buffer.
	 **/
	template<
		size_t I,
		typename ElemT = EElemT<I>
	>
	SOAWrite<ElemT> operator[](soa_index<I>)
	{
		return SOAWrite<ElemT>{eastl::get<I>(_tuple), _size};
	}
public:
	SOA()
	{
	}

	~SOA()
	{
		Clear();
	}

	/**
		\brief Retrieve the size of the container.

		Do I really need to explain this one?
	**/
	size_t Size() const
	{
		return _size;
	}

	/**
		Retrieve the index to the last element in the container.
	 **/
	size_t Last() const
	{
		return _size - 1;
	}

	/**
		\brief Retrieve the capacity of the container.

		This one I will explain. Capacity refers to how much space for how many elements this SOA
		can hold. Capacity is not related to Size in any way other than for bounds checking.
	**/
	size_t Capacity() const
	{
		return _capacity;
	}

	/**
		Retrieve the size in bytes of the container.

		\notes Used for unit testing, mostly. I dunno. Maybe you'll get some use out of it.
	 **/
	size_t Bytes() const
	{
		return _sizeBytes;
	}

private:
	template<size_t Index = 0>
	inline constexpr void ReserveIterate(char* element, size_t numMembers)
	{
		if constexpr(Index < LLength)
		{
			auto& memberPointer = eastl::get<Index>(_tuple);
			memberPointer = reinterpret_cast<decltype(memberPointer)>(element);

			// once we've stored nextBlock in the tuple, we simply need to move the nextElement pointer
			// to the location of the next block.
			size_t sizeofElement = sizeof(raw<decltype(memberPointer)>);
			size_t sizeofBlock = sizeofElement * numMembers;
			if constexpr(Index + 1 < LLength)
			{
				ReserveIterate<Index + 1>(element + sizeofBlock, numMembers);
			}
		}
	}
public:
	/**
		\brief Reserve space for \c numMembers elements in the internal storage buffer.

		This operation behaves in the same manner as \c eastl::vector, where.

		1) If \c numMembers > #SOA::Capacity, new storage is allocated and the old data is copied over.
		2) If \c numMembers <= #SOA::Capacity, no-op. Nothing happens.

		\arg \c numMembers The number of members you wish to allocate space for.

		\post Capacity == numMembers
		\post #SOA::Size remains unchanged.

		\note This operation will never decrease the capacity of the container. For that, check out #SOA::ShrinkToFit.
	 **/
	void Reserve(size_t numMembers)
	{
		FIRE_ASSERT_MSG(numMembers < FIRE_MAX_SOA_CAPACITY, "maximum capacity of the SOA container has been reached.");

		if(numMembers < _capacity) { return; }
		_capacity = numMembers;

		// we'll store this properly as void* later. referenced as char* for now to make offset
		// calculation a bit cleaner to look at (void* can't reliably have pointer arithmetic applied 
		// to it on all compilers)
		_sizeBytes = Sizeof * numMembers;
		char* newBuffer = (char*)fire_alloc(_sizeBytes);//libCore::Alloc<char>(_sizeBytes);
		FIRE_ASSERT(newBuffer != nullptr);

		// calculate the block offsets and store them in the tuple //
		char* nextElement = newBuffer;
		ReserveIterate(newBuffer, numMembers);

		if(_buffer)
		{
			// no sense in copying the entire old buffer, especially if _size is considerably less than the capacity.
			memcpy(newBuffer, _buffer, Sizeof * _size);
			fire_free(_buffer);
		}
		_buffer = newBuffer;
		_capacity = numMembers;
	}

private:
	// iterator for the PushBack operation.
	template<
		size_t Index = 0,
		class args_tuple,
		class ElementType = raw<eastl::tuple_element_t<Index, tuple_type>>
	>
	constexpr void PushBackIterate(size_t elementPos, args_tuple& args)
	{
		if constexpr(Index < LLength)
		{
			auto& element = eastl::get<Index>(_tuple);

			using bp_type = raw<decltype(element)>;
			if constexpr(RequiresConstructorCall<bp_type>())
			{
				new (element + elementPos) ElementType(eastl::get<Index>(args));
			}
			else
			{
				element[elementPos] = eastl::get<Index>(args);
			}

			if constexpr(Index + 1 < LLength)
			{
				PushBackIterate<Index + 1>(elementPos, args);
			}
		}
	}
public:
	/**
		\brief Insert an element into the back of the container.

		This function behaves just like \c eastl::vector::push_back() where elements pushed in
		will be at the back of the buffer. Attempting to push a data set when #SOA::Size() == #SOA::Capacity()
		will result in the capacity growing exponentially from its present capacity.

		\arg \c args A number of arguments to initialize each member with.
		\warning A static_assert will trigger if the number of arguments passed does not equal the number of
		members in this container.
	 **/
	template<class... Args>
	inline size_t PushBack(Args... args)
	{
		static_assert(sizeof...(Args) == LLength, "invalid number of arguments provided to PushBack");
		if(_size == _capacity)
		{
			if(_capacity == 0)
			{
				Reserve(1);
			}
			else
			{
				Reserve(_capacity * 2);
			}
		}
		
		PushBackIterate(_size, eastl::make_tuple(args...));

		size_t itemIndex = _size;
		++_size;
		return itemIndex;
	}

private:
	// iterator for the PushBack operation.
	template<
		size_t Index = 0,
		class ElementType = raw<eastl::tuple_element_t<Index, tuple_type>>
	>
	constexpr void PushBackPlainIterate(size_t elementPos)
	{
		if constexpr(Index < LLength)
		{
			auto& element = eastl::get<Index>(_tuple);

			using bp_type = raw<decltype(element)>;
			if constexpr(RequiresConstructorCall<bp_type>())
			{
				new(element + elementPos) ElementType();
			}

			if constexpr(Index + 1 < LLength)
			{
				PushBackPlainIterate<Index + 1>(elementPos);
			}
		}
	}
public:
	inline size_t PushBack()
	{
		if(_size == _capacity)
		{
			if(_capacity == 0)
			{
				Reserve(1);
			}
			else
			{
				Reserve(_capacity * 2);
			}
		}

		PushBackPlainIterate(_size);

		size_t itemIndex = _size;
		++_size;
		return itemIndex;
	}

private:
	// iterator for the Erase operation.
	template<size_t Index = 0>
	constexpr void EraseIterate(size_t removedElement)
	{
		if constexpr(Index < LLength)
		{
			auto& element = eastl::get<Index>(_tuple);

			using bp_type = raw<decltype(element)>;
			if constexpr(RequiresDestructorCall<bp_type>())
			{
				element[removedElement].~bp_type();
			}
			// no need to swap if we're erasing the last element.
			if(removedElement != Last())
			{
				eastl::swap(element[removedElement], element[Last()]);
			}

			if constexpr(Index + 1 < LLength)
			{
				EraseIterate<Index + 1>(removedElement);
			}
		}
	}

	// iterator for the Erase operation.
	template<size_t Index = 0>
	constexpr void EraseRangeIterate(size_t from, size_t to)
	{
		FIRE_ASSERT(from < _size && to < _size && to > from);
		if constexpr(Index < LLength)
		{
			auto& element = eastl::get<Index>(_tuple);

			using bp_type = raw<decltype(element)>;
			if constexpr(RequiresDestructorCall<bp_type>())
			{
				for(size_t i=from; i<=to; ++i)
				{
					element[removedElement].~bp_type();
				}
			}
			else
			{
				FIRE_ASSERT((to - from) * Sizeof < _sizeBytes);
				memset(&element[from], 0, (to - from) * Sizeof);
			}
			
			if constexpr(Index + 1 < LLength)
			{
				EraseIterate<Index + 1>(removedElement);
			}
		}
	}
public:
	/**
		\brief Remove an element at the specified index.

		This operation will remove an element at the specified index by swapping the values stored at
		\c #SOA::Size() - 1 with the provided index. As a result, this will invalidate the specified index 
		as new data will be stored there. The swap occurs with \c eastl::swap, so elements must meet the requirements
		that \c eastl::swap imposes.

		\arg \c index The index of the element that you want to erase.

		\return \c lastElementIndex The index of the last element (prior to the decrementing of the size) is returned
		so that you can perform any housekeeping routines that would result from this operation.

		\post #SOA::Size() == #SOA::Size - 1

		\note Calling this when size == 0 is a no-op.
		\warning Will assert if \c index \c >= \c size.
	 **/
	inline size_t Erase(size_t index)
	{
		if(_size == 0) { return 0; }
		FIRE_ASSERT_MSG(index < _size, Format("index '%d' out of bounds", index).c_str());
		EraseIterate(index);
		size_t out = _size;
		--_size;
		return _size;
	}


private:
	// iterator for the Erase operation.
	template<size_t Index = 0>
	constexpr void SwapIterate(size_t i1, size_t i2)
	{
		if constexpr(Index < LLength)
		{
			auto& element = eastl::get<Index>(_tuple);
			eastl::swap(element[i1], element[i2]);

			if constexpr(Index + 1 < LLength)
			{
				SwapIterate<Index + 1>(i1, i2);
			}
		}
	}
public:
	inline void Swap(size_t i1, size_t i2)
	{
		if(_size == 0) { return; }
		if(i1 == i2) { return; }
		FIRE_ASSERT_MSG(i1 < _size, Format("index '%d' out of bounds", i1).c_str());
		FIRE_ASSERT_MSG(i2 < _size, Format("index '%d' out of bounds", i2).c_str());
		SwapIterate(i1, i2);
	}

	/**
		Shrink the size of the memory buffers so that #SOA::Size() == #SOA::Capacity(). This will trigger a 
		new allocation for the shrunk buffer and copy all of the old data over.

		\note Calling this function when #SOA::Size == #SOA::Capacity is a no-op.
	 **/
	inline void ShrinkToFit()
	{
		if(_capacity == _size) { return; }

		size_t sizeofNew = _size * Sizeof;
		char* oldBuffer = _buffer;
		char* newBuffer = fire_alloc(sizeofNew);
		memmove(newBuffer, _buffer, sizeofNew);

		_buffer = (void*)newBuffer;

		libCore::Free(oldBuffer);
		_capacity = _size;
	}

	/**
		\brief Return whether or not the container is empty.

		'Nuff said.

		\return \c isEmpty Whether or not the container is eqop[weitfnq;eovfuihwer[otn13rgv...
	 **/
	inline bool Empty() const
	{
		return _size == 0;
	}

private:
	// iterator for the Erase operation.
	template<size_t Index = 0>
	constexpr void ClearIterate()
	{
		if constexpr(Index < LLength)
		{
			auto& element = eastl::get<Index>(_tuple);
			element = nullptr;

			if constexpr(Index + 1 < LLength)
			{
				ClearIterate<Index + 1>();
			}
		}
	}
public:
	/**
		\brief Deallocate the buffer memory.

		This will deallocate all the memory that the SOA has stored and set the size and capacity to 0.

		\post #SOA::Capacity == 0
		\post #SOA::Size == 0
	 **/
	inline void Clear()
	{
		if(_buffer)
		{
			memset(_buffer, 0, _sizeBytes);
			libCore::Free(_buffer);
			ClearIterate();
			_size = 0;
			_capacity = 0;
			_buffer = nullptr;
		}
	}
};
#endif
CLOSE_NAMESPACE(Firestorm);
#endif
