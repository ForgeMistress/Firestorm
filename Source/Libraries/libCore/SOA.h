///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SOA
//
//  A generalized implementation of a structure of arrays.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
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

// Use this to make a named static Tuple Vector Index for easier indexing into the SOA.
#define FIRE_TVI(SYMBOL, POS) static constexpr soa_index<POS> SYMBOL{};

template <char... Digits>
decltype(auto) operator"" _soa()
{
	return soa_index<parse<Digits...>()>{};
}

template<char... Digits>
decltype(auto) operator"" _tvi()
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


// MIT License
// 
// Copyright (c) 2018 Tsung-Wei Huang, Chun-Xun Lin, and Martin Wong
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <iostream>
#include <mutex>
#include <deque>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <list>
#include <forward_list>
#include <numeric>
#include <iomanip>
#include <cassert>
#include <optional>

#include "threadpool/threadpool.hpp"

// ============================================================================

// Clang mis-interprets variant's get as a non-friend of variant and cannot
// get compiled correctly. We use the patch: 
// https://gcc.gnu.org/viewcvs/gcc?view=revision&revision=258854
// to get rid of this.
#if defined(__clang__)
#include "patch/clang_variant.hpp"
#else
#include <variant>
#endif

// ============================================================================


namespace tf {

// Procedure: throw_re
template <typename... ArgsT>
inline void throw_re(const char* fname, const size_t line, ArgsT&&... args) {
	std::ostringstream oss(std::ios_base::out);
	oss << '[' << fname << ':' << line << "] ";
	(oss << ... << std::forward<ArgsT>(args));
	throw std::runtime_error(oss.str());
}

#define TF_THROW(...) throw_re(__FILE__, __LINE__, __VA_ARGS__);

//-----------------------------------------------------------------------------
// Traits
//-----------------------------------------------------------------------------

// Macro to check whether a class has a member function
#define define_has_member(member_name)                                     \
template <typename T>                                                      \
class has_member_##member_name                                             \
{                                                                          \
  typedef char yes_type;                                                   \
  typedef long no_type;                                                    \
  template <typename U> static yes_type test(decltype(&U::member_name));   \
  template <typename U> static no_type  test(...);                         \
  public:                                                                  \
    static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);  \
}

#define has_member(class_, member_name)  has_member_##member_name<class_>::value

// Struct: dependent_false
template <typename... T>
struct dependent_false { 
	static constexpr bool value = false; 
};

template <typename... T>
constexpr auto dependent_false_v = dependent_false<T...>::value;

// Struct: is_iterator
template <typename T, typename = void>
struct is_iterator {
	static constexpr bool value = false;
};

template <typename T>
struct is_iterator<
	T, 
	std::enable_if_t<!std::is_same_v<typename std::iterator_traits<T>::value_type, void>>
> {
	static constexpr bool value = true;
};

template <typename T>
inline constexpr bool is_iterator_v = is_iterator<T>::value;

// Struct: is_iterable
template <typename T, typename = void>
struct is_iterable : std::false_type {
};

template <typename T>
struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin()),
	decltype(std::declval<T>().end())>>
	: std::true_type {
};

template <typename T>
inline constexpr bool is_iterable_v = is_iterable<T>::value;

//-----------------------------------------------------------------------------
// Taskflow definition
//-----------------------------------------------------------------------------

// tweakable parameters

using DependentsCountType = int;

// Forward declaration
class Node;
class Topology;
class Task;
class FlowBuilder;
class SubflowBuilder;
class Taskflow;

using Graph = std::forward_list<Node>;

using NodeID = size_t;

template<typename NodeType>
struct ForgeGraph
{
	// move only
	ForgeGraph(ForgeGraph<NodeType>&& other)
		: _nodeNames(std::move(other._nodeNames))
		, _poolLock(std::move(other._poolLock))
		, _nodePool(std::move(other._nodePool))
		, _recycleLock(std::move(other._recycleLock))
		, _recycle(std::move(other._recycle))
	{}

	NodeID MakeNode();

	template<typename C>
	NodeID MakeNode(C&&);

	void ReleaseNode(NodeID node);

	const std::string& get_name(NodeID node) {
		return _nodeNames[node];
	}

	const std::string& set_name(NodeID node, const char* name) {
		_nodeNames[node] = name;
	}
	size_t size() const {
		std::scoped_lock lock(_poolLock);
		return _nodePool.size();
	}

	Node& get(NodeID node)
	{
		return _nodePool[node];
	}

	std::deque<std::string> _nodeNames;
	std::mutex _poolLock;
	std::deque<NodeType> _nodePool;
	std::mutex _recycleLock;
	std::deque<NodeID> _recycle;
};

template<typename NodeType>
NodeID ForgeGraph<NodeType>::MakeNode()
{
	_recycleLock.lock();
	if(_recycle.empty())
	{
		_recycleLock.unlock();

		_poolLock.lock();
		NodeID nodeID = _nodePool.size();
		Node& newNode = _nodePool.emplace_back();
		_nodeNames.push_back("");
		newNode.id = nodeID;
		_poolLock.unlock();
		return nodeID;
	}
	NodeID nodeID = _recycle.back();
	_recycle.pop_back();
	_recycleLock.unlock();
	_poolLock.lock();
	new (_nodePool[nodeID]) NodeType();
	_poolLock.unlock();
	return nodeID;
}

template<typename NodeType>
template<typename C>
NodeID ForgeGraph<NodeType>::MakeNode(C&& c)
{
	_recycleLock.lock();
	if(_recycle.empty())
	{
		_recycleLock.unlock();

		_poolLock.lock();
		NodeID nodeID = _nodePool.size();
		Node& newNode = _nodePool.emplace_back(std::forward<C>(c));
		_nodeNames.push_back("");
		newNode.id = nodeID;
		_poolLock.unlock();
		return nodeID;
	}
	NodeID nodeID = _recycle.back();
	_recycle.pop_back();
	_recycleLock.unlock();
	_poolLock.lock();
	new (_nodePool[nodeID]) NodeType(std::forward<C>(c));
	_poolLock.unlock();
	return nodeID;
}

template<typename NodeType>
void ForgeGraph<NodeType>::ReleaseNode(NodeID node)
{
	_recycleLock.lock();
	_poolLock.lock();
	_nodePool[node].~NodeType();
	_poolLock.unlock();
	_recycle.push_back(node);
	_recycleLock.unlock();
}

// ----------------------------------------------------------------------------

// Class: Node
class Node {

	friend class Task;
	friend class Topology;
	friend class Taskflow;

	using StaticWork   = std::function<void()>;
	using DynamicWork  = std::function<void(SubflowBuilder&)>;

public:

	Node(ForgeGraph<Node>& pool);

	template <typename C>
	explicit Node(ForgeGraph<Node>& pool, C&&);

	const std::string& name() const;
	void set_name(const char* name);

	void precede(Node&);

	size_t num_successors() const;
	size_t num_dependents() const;

	std::string dump() const;

private:
	friend class ForgeGraph<Node>;
	ForgeGraph<Node>& _nodePool; // for retrieving the name.
	NodeID id;
	//std::string _name;
	std::variant<StaticWork, DynamicWork> _work;
	std::vector<Node*> _successors;
	std::atomic<DependentsCountType> _dependents;

	std::optional<Graph> _subgraph;

	Topology* _topology;

	void _dump(std::ostream&) const;
};

// Constructor
inline Node::Node(ForgeGraph<Node>& nodePool) : _nodePool(nodePool) {
	_dependents.store(0, std::memory_order_relaxed);
	_topology = nullptr;
}

// Constructor
template <typename C>
inline Node::Node(ForgeGraph<Node>& nodePool, C&& c): _nodePool{ nodePool }, _work { std::forward<C>(c) } {
	_dependents.store(0, std::memory_order_relaxed);
	_topology = nullptr;
}

// Procedure: precede
inline void Node::precede(Node& v) {
	_successors.push_back(&v);
	v._dependents.fetch_add(1, std::memory_order_relaxed);
}

// Function: num_successors
inline size_t Node::num_successors() const {
	return _successors.size();
}

// Function: dependents
inline size_t Node::num_dependents() const {
	return _dependents.load(std::memory_order_relaxed);
}

// Function: name
inline const std::string& Node::name() const {
	return _nodePool.get_name(id);
}

inline void Node::set_name(const char* name) {
	_nodePool.set_name(id, name);
}

// Function: dump
inline std::string Node::dump() const {
	std::ostringstream os;  
	_dump(os);
	return os.str();
}

// Function: _dump
inline void Node::_dump(std::ostream& os) const {
	const auto& _name = name();
	if(_name.empty()) os << '\"' << this << '\"';
	else os << std::quoted(_name);
	os << ";\n";

	for(const auto s : _successors) {

		if(_name.empty()) os << '\"' << this << '\"';
		else os << std::quoted(_name);

		os << " -> ";

		if(s->name().empty()) os << '\"' << s << '\"';
		else os << std::quoted(s->name());

		os << ";\n";
	}

	if(_subgraph && !_subgraph->empty()) {

		os << "subgraph cluster_";
		if(_name.empty()) os << this;
		else os << _name;
		os << " {\n";

		os << "label = \"Subflow_";
		if(_name.empty()) os << this;
		else os << _name;

		os << "\";\n" << "color=blue\n";

		for(const auto& n : *_subgraph) {
			n._dump(os);
		}
		os << "}\n";
	}
}

// ----------------------------------------------------------------------------

// class: Topology
class Topology {

	friend class Taskflow;

public:

	Topology(ForgeGraph<Node>&&);

	std::string dump() const;

private:

	ForgeGraph<Node> _graph;

	std::shared_future<void> _future;

	NodeID _source;
	NodeID _target;

	void _dump(std::ostream&) const;
};

// Constructor
inline Topology::Topology(ForgeGraph<Node>&& t) : 
	_graph(std::move(t)),
	_source(_graph.MakeNode()),
	_target(_graph.MakeNode()) {

	_source._topology = this;
	_target._topology = this;

	std::promise<void> promise;

	_future = promise.get_future().share();

	_target._work = [p=MoC{std::move(promise)}] () mutable { 
		p.get().set_value(); 
	};

	// ensure the topology is connected
	_source.precede(_target);

	// Build the super source and super target.
	for(auto& node : _graph) {

		node._topology = this;

		if(node.num_dependents() == 0) {
			_source.precede(node);
		}

		if(node.num_successors() == 0) {
			node.precede(_target);
		}
	}
}

// Procedure: _dump
inline void Topology::_dump(std::ostream& os) const {

	assert(_source._subgraph->empty());
	assert(_target._subgraph->empty());

	os << "digraph Topology {\n"
		<< _source.dump() 
		<< _target.dump();

	for(const auto& node : _graph) {
		os << node.dump();
	}

	os << "}\n";
}

// Function: dump
inline std::string Topology::dump() const { 
	std::ostringstream os;
	_dump(os);
	return os.str();
}

// ----------------------------------------------------------------------------

// Class: Task
class Task {

	friend class FlowBuilder;
	friend class Taskflow;

public:

	Task() = default;
	Task(Node&);
	Task(const Task&);
	Task(Task&&);

	Task& operator = (const Task&);

	const std::string& name() const;

	size_t num_successors() const;
	size_t num_dependents() const;

	Task& name(const std::string&);
	Task& precede(Task);
	Task& broadcast(std::vector<Task>&);
	Task& broadcast(std::initializer_list<Task>);
	Task& gather(std::vector<Task>&);
	Task& gather(std::initializer_list<Task>);

	template <typename C>
	Task& work(C&&);

	template <typename... Bs>
	Task& broadcast(Bs&&...);

	template <typename... Bs>
	Task& gather(Bs&&...);

private:

	Node* _node {nullptr};

	template<typename S>
	void _broadcast(S&);

	template<typename S>
	void _gather(S&);
};

// Constructor
inline Task::Task(Node& t) : _node {&t} {
}

// Constructor
inline Task::Task(const Task& rhs) : _node {rhs._node} {
}

// Function: precede
inline Task& Task::precede(Task tgt) {
	_node->precede(*(tgt._node));
	return *this;
}

// Function: broadcast
template <typename... Bs>
inline Task& Task::broadcast(Bs&&... tgts) {
	(_node->precede(*(tgts._node)), ...);
	return *this;
}

// Procedure: _broadcast
template <typename S>
inline void Task::_broadcast(S& tgts) {
	for(auto& to : tgts) {
		_node->precede(*(to._node));
	}
}

// Function: broadcast
inline Task& Task::broadcast(std::vector<Task>& tgts) {
	_broadcast(tgts);
	return *this;
}

// Function: broadcast
inline Task& Task::broadcast(std::initializer_list<Task> tgts) {
	_broadcast(tgts);
	return *this;
}

// Function: gather
template <typename... Bs>
inline Task& Task::gather(Bs&&... tgts) {
	(tgts.precede(*this), ...);
	return *this;
}

// Procedure: _gather
template <typename S>
inline void Task::_gather(S& tgts) {
	for(auto& from : tgts) {
		from._node->precede(*_node);
	}
}

// Function: gather
inline Task& Task::gather(std::vector<Task>& tgts) {
	_gather(tgts);
	return *this;
}

// Function: gather
inline Task& Task::gather(std::initializer_list<Task> tgts) {
	_gather(tgts);
	return *this;
}

// Operator =
inline Task& Task::operator = (const Task& rhs) {
	_node = rhs._node;
	return *this;
}

// Constructor
inline Task::Task(Task&& rhs) : _node{rhs._node} { 
	rhs._node = nullptr; 
}

// Function: work
template <typename C>
inline Task& Task::work(C&& c) {
	_node->_work = std::forward<C>(c);
	return *this;
}

// Function: name
inline Task& Task::name(const std::string& name) {
	_node->set_name(name.c_str());
	return *this;
}

// Function: name
inline const std::string& Task::name() const {
	return _node->name();
}

// Function: num_dependents
inline size_t Task::num_dependents() const {
	return _node->_dependents.load(std::memory_order_relaxed);
}

// Function: num_successors
inline size_t Task::num_successors() const {
	return _node->_successors.size();
}

// ----------------------------------------------------------------------------

// Class: FlowBuilder
class FlowBuilder {

public:

	FlowBuilder(ForgeGraph<Node>&, size_t);

	template <typename C>
	auto emplace(C&&);

	template <typename... C, std::enable_if_t<(sizeof...(C)>1), void>* = nullptr>
	auto emplace(C&&...);

	template <typename C>
	auto silent_emplace(C&&);

	template <typename... C, std::enable_if_t<(sizeof...(C)>1), void>* = nullptr>
	auto silent_emplace(C&&...);

	template <typename I, typename C>
	auto parallel_for(I, I, C&&, size_t = 0);

	template <typename T, typename C, std::enable_if_t<is_iterable_v<T>, void>* = nullptr>
	auto parallel_for(T&, C&&, size_t = 0);

	template <typename I, typename T, typename B>
	auto reduce(I, I, T&, B&&);

	template <typename I, typename T>
	auto reduce_min(I, I, T&);

	template <typename I, typename T>
	auto reduce_max(I, I, T&);

	template <typename I, typename T, typename B, typename U>
	auto transform_reduce(I, I, T&, B&&, U&&);

	template <typename I, typename T, typename B, typename P, typename U>
	auto transform_reduce(I, I, T&, B&&, P&&, U&&);

	auto placeholder();

	void precede(Task, Task);
	void linearize(std::vector<Task>&);
	void linearize(std::initializer_list<Task>);
	void broadcast(Task, std::vector<Task>&);
	void broadcast(Task, std::initializer_list<Task>);
	void gather(std::vector<Task>&, Task);
	void gather(std::initializer_list<Task>, Task);  

	size_t size() const;

	bool empty() const;

protected:

	ForgeGraph<Node>& _graph;

	size_t _partition_factor;

	template <typename L>
	void _linearize(L&);
};

inline FlowBuilder::FlowBuilder(ForgeGraph<Node>& graph, size_t f) : 
	_graph {graph}, 
	_partition_factor {f} {
}    

// Procedure: size
inline size_t FlowBuilder::size() const {
	return _graph.size();
	//return std::distance(_graph.begin(), _graph.end());
}

// Function: empty
inline bool FlowBuilder::empty() const {
	return _graph.empty();
}

// Procedure: precede
inline void FlowBuilder::precede(Task from, Task to) {
	from._node->precede(*(to._node));
}

// Procedure: broadcast
inline void FlowBuilder::broadcast(Task from, std::vector<Task>& keys) {
	from.broadcast(keys);
}

// Procedure: broadcast
inline void FlowBuilder::broadcast(
	Task from, std::initializer_list<Task> keys
) {
	from.broadcast(keys);
}

// Function: gather
inline void FlowBuilder::gather(
	std::vector<Task>& keys, Task to
) {
	to.gather(keys);
}

// Function: gather
inline void FlowBuilder::gather(
	std::initializer_list<Task> keys, Task to
) {
	to.gather(keys);
}

// Function: placeholder
inline auto FlowBuilder::placeholder() {
	auto& node = _graph.emplace_front();
	return Task(node);
}

// Function: silent_emplace
template <typename... C, std::enable_if_t<(sizeof...(C)>1), void>*>
				inline auto FlowBuilder::silent_emplace(C&&... cs) {
					return std::make_tuple(silent_emplace(std::forward<C>(cs))...);
				}


				// Function: parallel_for    
				template <typename I, typename C>
				inline auto FlowBuilder::parallel_for(I beg, I end, C&& c, size_t g) {

					using category = typename std::iterator_traits<I>::iterator_category;

					if(g == 0) {
						auto d = std::distance(beg, end);
						auto w = std::max(size_t{1}, _partition_factor);
						g = (d + w - 1) / w;
					}

					auto source = placeholder();
					auto target = placeholder();

					while(beg != end) {

						auto e = beg;

						// Case 1: random access iterator
						if constexpr(std::is_same_v<category, std::random_access_iterator_tag>) {
							size_t r = std::distance(beg, end);
							std::advance(e, std::min(r, g));
						}
						// Case 2: non-random access iterator
						else {
							for(size_t i=0; i<g && e != end; ++e, ++i);
						}

						// Create a task
						auto task = silent_emplace([beg, e, c] () mutable {
							std::for_each(beg, e, c);
						});
						source.precede(task);
						task.precede(target);

						// adjust the pointer
						beg = e;
					}

					return std::make_pair(source, target); 
				}

				// Function: parallel_for
				template <typename T, typename C, std::enable_if_t<is_iterable_v<T>, void>*>
				inline auto FlowBuilder::parallel_for(T& t, C&& c, size_t group) {
					return parallel_for(t.begin(), t.end(), std::forward<C>(c), group);
				}

				// Function: reduce_min
				// Find the minimum element over a range of items.
				template <typename I, typename T>
				inline auto FlowBuilder::reduce_min(I beg, I end, T& result) {
					return reduce(beg, end, result, [] (const auto& l, const auto& r) {
						return std::min(l, r);
					});
				}

				// Function: reduce_max
				// Find the maximum element over a range of items.
				template <typename I, typename T>
				inline auto FlowBuilder::reduce_max(I beg, I end, T& result) {
					return reduce(beg, end, result, [] (const auto& l, const auto& r) {
						return std::max(l, r);
					});
				}

				// Function: transform_reduce    
				template <typename I, typename T, typename B, typename U>
				inline auto FlowBuilder::transform_reduce(
					I beg, I end, T& result, B&& bop, U&& uop
				) {

					using category = typename std::iterator_traits<I>::iterator_category;

					// Even partition
					size_t d = std::distance(beg, end);
					size_t w = std::max(size_t{1}, _partition_factor);
					size_t g = std::max((d + w - 1) / w, size_t{2});

					auto source = placeholder();
					auto target = placeholder();

					std::vector<std::future<T>> futures;

					while(beg != end) {

						auto e = beg;

						// Case 1: random access iterator
						if constexpr(std::is_same_v<category, std::random_access_iterator_tag>) {
							size_t r = std::distance(beg, end);
							std::advance(e, std::min(r, g));
						}
						// Case 2: non-random access iterator
						else {
							for(size_t i=0; i<g && e != end; ++e, ++i);
						}

						// Create a task
						auto [task, future] = emplace([beg, e, bop, uop] () mutable {
							auto init = uop(*beg);
							for(++beg; beg != e; ++beg) {
								init = bop(std::move(init), uop(*beg));          
							}
							return init;
						});
						source.precede(task);
						task.precede(target);
						futures.push_back(std::move(future));

						// adjust the pointer
						beg = e;
					}

					// target synchronizer
					target.work([&result, futures=MoC{std::move(futures)}, bop] () {
						for(auto& fu : futures.object) {
							result = bop(std::move(result), fu.get());
						}
					});

					return std::make_pair(source, target); 
				}

				// Function: transform_reduce    
				template <typename I, typename T, typename B, typename P, typename U>
				inline auto FlowBuilder::transform_reduce(
					I beg, I end, T& result, B&& bop, P&& pop, U&& uop
				) {

					using category = typename std::iterator_traits<I>::iterator_category;

					// Even partition
					size_t d = std::distance(beg, end);
					size_t w = std::max(size_t{1}, _partition_factor);
					size_t g = std::max((d + w - 1) / w, size_t{2});

					auto source = placeholder();
					auto target = placeholder();

					std::vector<std::future<T>> futures;

					while(beg != end) {

						auto e = beg;

						// Case 1: random access iterator
						if constexpr(std::is_same_v<category, std::random_access_iterator_tag>) {
							size_t r = std::distance(beg, end);
							std::advance(e, std::min(r, g));
						}
						// Case 2: non-random access iterator
						else {
							for(size_t i=0; i<g && e != end; ++e, ++i);
						}

						// Create a task
						auto [task, future] = emplace([beg, e, uop, pop] () mutable {
							auto init = uop(*beg);
							for(++beg; beg != e; ++beg) {
								init = pop(std::move(init), *beg);
							}
							return init;
						});
						source.precede(task);
						task.precede(target);
						futures.push_back(std::move(future));

						// adjust the pointer
						beg = e;
					}

					// target synchronizer
					target.work([&result, futures=MoC{std::move(futures)}, bop] () {
						for(auto& fu : futures.object) {
							result = bop(std::move(result), fu.get());
						}
					});

					return std::make_pair(source, target); 
				}


				// Procedure: _linearize
				template <typename L>
				inline void FlowBuilder::_linearize(L& keys) {
					(void) std::adjacent_find(
						keys.begin(), keys.end(), 
						[] (auto& from, auto& to) {
						from._node->precede(*(to._node));
						return false;
					}
					);
				}

				// Procedure: linearize
				inline void FlowBuilder::linearize(std::vector<Task>& keys) {
					_linearize(keys); 
				}

				// Procedure: linearize
				inline void FlowBuilder::linearize(std::initializer_list<Task> keys) {
					_linearize(keys);
				}

				// Proceduer: reduce
				template <typename I, typename T, typename B>
				inline auto FlowBuilder::reduce(I beg, I end, T& result, B&& op) {

					using category = typename std::iterator_traits<I>::iterator_category;

					size_t d = std::distance(beg, end);
					size_t w = std::max(size_t{1}, _partition_factor);
					size_t g = std::max((d + w - 1) / w, size_t{2});

					auto source = placeholder();
					auto target = placeholder();

					std::vector<std::future<T>> futures;

					while(beg != end) {

						auto e = beg;

						// Case 1: random access iterator
						if constexpr(std::is_same_v<category, std::random_access_iterator_tag>) {
							size_t r = std::distance(beg, end);
							std::advance(e, std::min(r, g));
						}
						// Case 2: non-random access iterator
						else {
							for(size_t i=0; i<g && e != end; ++e, ++i);
						}

						// Create a task
						auto [task, future] = emplace([beg, e, op] () mutable {
							auto init = *beg;
							for(++beg; beg != e; ++beg) {
								init = op(std::move(init), *beg);          
							}
							return init;
						});
						source.precede(task);
						task.precede(target);
						futures.push_back(std::move(future));

						// adjust the pointer
						beg = e;
					}

					// target synchronizer
					target.work([&result, futures=MoC{std::move(futures)}, op] () {
						for(auto& fu : futures.object) {
							result = op(std::move(result), fu.get());
						}
					});

					return std::make_pair(source, target); 
				}

				// ----------------------------------------------------------------------------

				// Class: SubflowBuilder
				class SubflowBuilder : public FlowBuilder {

				public:

					template <typename... Args>
					SubflowBuilder(Args&&...);

					void join();
					void detach();

					bool detached() const;
					bool joined() const;

				private:

					bool _detached {false};
				};

				// Constructor
				template <typename... Args>
				inline SubflowBuilder::SubflowBuilder(Args&&... args) :
					FlowBuilder {std::forward<Args>(args)...} {
				}

				// Procedure: join
				inline void SubflowBuilder::join() {
					_detached = false;
				}

				// Procedure: detach
				inline void SubflowBuilder::detach() {
					_detached = true;
				}

				// Function: detached
				inline bool SubflowBuilder::detached() const {
					return _detached;
				}

				// Function: joined
				inline bool SubflowBuilder::joined() const {
					return !_detached;
				}

				// Function: emplace
				template <typename C>
				inline auto FlowBuilder::emplace(C&& c) {

					// subflow task
					if constexpr(std::is_invocable_v<C, SubflowBuilder&>) {

						using R = std::invoke_result_t<C, SubflowBuilder&>;
						std::promise<R> p;
						auto fu = p.get_future();

						if constexpr(std::is_same_v<void, R>) {
							auto& node = _graph.emplace_front([p=MoC(std::move(p)), c=std::forward<C>(c)]
							(SubflowBuilder& fb) mutable {
								if(fb._graph.empty()) {
									c(fb);
									if(fb.detached()) {
										p.get().set_value();
									}
								}
								else {
									p.get().set_value();
								}
							});
							return std::make_pair(Task(node), std::move(fu));
						}
						else {
							auto& node = _graph.emplace_front(
								[p=MoC(std::move(p)), c=std::forward<C>(c), r=std::optional<R>()]
							(SubflowBuilder& fb) mutable {
								if(fb._graph.empty()) {
									r.emplace(c(fb));
									if(fb.detached()) {
										p.get().set_value(std::move(*r)); 
									}
								}
								else {
									assert(r);
									p.get().set_value(std::move(*r));
								}
							});
							return std::make_pair(Task(node), std::move(fu));
						}
					}
					// regular task
					else if constexpr(std::is_invocable_v<C>) {

						using R = std::invoke_result_t<C>;
						std::promise<R> p;
						auto fu = p.get_future();

						if constexpr(std::is_same_v<void, R>) {
							auto& node = _graph.emplace_front(
								[p=MoC(std::move(p)), c=std::forward<C>(c)]() mutable {
								c(); 
								p.get().set_value();
							}
							);
							return std::make_pair(Task(node), std::move(fu));
						}
						else {
							auto& node = _graph.emplace_front(
								[p=MoC(std::move(p)), c=std::forward<C>(c)]() mutable {
								p.get().set_value(c());
							}
							);
							return std::make_pair(Task(node), std::move(fu));
						}
					}
					else {
						static_assert(dependent_false_v<C>, "invalid task work type");
					}
				}

				// Function: emplace
				template <typename... C, std::enable_if_t<(sizeof...(C)>1), void>*>
								inline auto FlowBuilder::emplace(C&&... cs) {
									return std::make_tuple(emplace(std::forward<C>(cs))...);
								}

								// Function: silent_emplace
								template <typename C>
								inline auto FlowBuilder::silent_emplace(C&& c) {
									// subflow task
									if constexpr(std::is_invocable_v<C, SubflowBuilder&>) {
										auto& n = _graph.emplace_front(
											[c=std::forward<C>(c)] (SubflowBuilder& fb) {
											// first time execution
											if(fb._graph.empty()) {
												c(fb);
											}
										});
										return Task(n);
									}
									// regular task
									else if constexpr(std::is_invocable_v<C>) {
										auto& n = _graph.emplace_front(std::forward<C>(c));
										return Task(n);
									}
									else {
										static_assert(dependent_false_v<C>, "invalid task work type");
									}
								}

								// ============================================================================
								// Taskflow Definition
								// ============================================================================

								// Class: Taskflow
								class Taskflow : public FlowBuilder {

									using StaticWork  = typename Node::StaticWork;
									using DynamicWork = typename Node::DynamicWork;

									// Closure
									struct Closure {

										Closure() = default;
										Closure(const Closure&) = delete;
										Closure(Closure&&);
										Closure(Taskflow&, Node&);

										Closure& operator = (Closure&&);
										Closure& operator = (const Closure&) = delete;

										void operator ()() const;

										Taskflow* taskflow {nullptr};
										Node*     node     {nullptr};
									};

								public:

									explicit Taskflow();
									explicit Taskflow(unsigned);

									~Taskflow();

									std::shared_future<void> dispatch();

									void silent_dispatch();
									void wait_for_all();
									void wait_for_topologies();
									void dump(std::ostream&) const;

									size_t num_nodes() const;
									size_t num_workers() const;
									size_t num_topologies() const;

									std::string dump() const;
									std::string dump_topologies() const;

								private:

									SpeculativeThreadpool<Closure> _executor;

									Graph _graph;

									std::forward_list<Topology> _topologies;

									void _schedule(Node&);
								};

								// Constructor    
								inline Taskflow::Closure::Closure(Closure&& rhs) : 
									taskflow {rhs.taskflow}, node {rhs.node} { 
									rhs.taskflow = nullptr;
									rhs.node     = nullptr;
								}

								// Constructor
								inline Taskflow::Closure::Closure(Taskflow& t, Node& n) : 
									taskflow{&t}, node {&n} {
								}

								// Move assignment
								inline Taskflow::Closure& Taskflow::Closure::operator = (Closure&& rhs) {
									taskflow = rhs.taskflow;
									node     = rhs.node;
									rhs.taskflow = nullptr;
									rhs.node     = nullptr;
									return *this;
								}

								// Operator ()
								inline void Taskflow::Closure::operator () () const {

									assert(taskflow && node);

									// Here we need to fetch the num_successors first to avoid the invalid memory
									// access caused by topology clear.
									const auto num_successors = node->num_successors();

									// regular node type
									// The default node work type. We only need to execute the callback if any.
									if(auto index=node->_work.index(); index == 0) {
										if(auto &f = std::get<StaticWork>(node->_work); f != nullptr){
											std::invoke(f);
										}
									}
									// subflow node type 
									// The first time we enter into the subflow context, 
									// "subnodes" must be empty.
									// After executing the user's callback on subflow, 
									// there will be at least one node node used as "super source". 
									// The second time we enter this context there is no need
									// to re-execute the work.
									else {
										assert(std::holds_alternative<DynamicWork>(node->_work));

										if(!node->_subgraph.has_value()){
											node->_subgraph.emplace();  // Initialize the _subgraph		
										}

										SubflowBuilder fb(*(node->_subgraph), taskflow->num_workers());

										bool empty_graph = node->_subgraph->empty();

										std::invoke(std::get<DynamicWork>(node->_work), fb);

										// Need to create a subflow
										if(empty_graph) {

											auto& S = node->_subgraph->emplace_front([](){});

											S._topology = node->_topology;

											for(auto i = std::next(node->_subgraph->begin()); i != node->_subgraph->end(); ++i) {

												i->_topology = node->_topology;

												if(i->num_successors() == 0) {
													i->precede(fb.detached() ? node->_topology->_target : *node);
												}

												if(i->num_dependents() == 0) {
													S.precede(*i);
												}
											}

											// this is for the case where subflow graph might be empty
											if(!fb.detached()) {
												S.precede(*node);
											}

											taskflow->_schedule(S);

											if(!fb.detached()) {
												return;
											}
										}
									}

									// At this point, the node/node storage might be destructed.
									for(size_t i=0; i<num_successors; ++i) {
										if(--(node->_successors[i]->_dependents) == 0) {
											taskflow->_schedule(*(node->_successors[i]));
										}
									}
								}

								// Constructor
								inline Taskflow::Taskflow() : 
									FlowBuilder {_graph, std::thread::hardware_concurrency()},
									_executor   {std::thread::hardware_concurrency()} {
								}

								// Constructor
								inline Taskflow::Taskflow(unsigned N) : 
									FlowBuilder {_graph, N},
									_executor   {N} {
								}

								// Destructor
								inline Taskflow::~Taskflow() {
									wait_for_topologies();
								}

								// Function: num_nodes
								inline size_t Taskflow::num_nodes() const {
									//return _nodes.size();
									return std::distance(_graph.begin(), _graph.end());
								}

								// Function: num_workers
								inline size_t Taskflow::num_workers() const {
									return _executor.num_workers();
								}

								// Function: num_topologies
								inline size_t Taskflow::num_topologies() const {
									return std::distance(_topologies.begin(), _topologies.end());
								}

								// Procedure: silent_dispatch 
								inline void Taskflow::silent_dispatch() {

									if(_graph.empty()) return;

									auto& topology = _topologies.emplace_front(std::move(_graph));

									// Start the taskflow
									_schedule(topology._source);
								}

								// Procedure: dispatch 
								inline std::shared_future<void> Taskflow::dispatch() {

									if(_graph.empty()) {
										return std::async(std::launch::deferred, [](){}).share();
									}

									auto& topology = _topologies.emplace_front(std::move(_graph));

									// Start the taskflow
									_schedule(topology._source);

									return topology._future;
								}

								// Procedure: wait_for_all
								inline void Taskflow::wait_for_all() {
									if(!_graph.empty()) {
										silent_dispatch();
									}
									wait_for_topologies();
								}

								// Procedure: wait_for_topologies
								inline void Taskflow::wait_for_topologies() {
									for(auto& t: _topologies){
										t._future.get();
									}
									_topologies.clear();
								}

								// Procedure: _schedule
								// The main procedure to schedule a give task node.
								// Each task node has two types of tasks - regular and subflow.
								inline void Taskflow::_schedule(Node& node) {
									_executor.emplace(*this, node);
								}

								// Function: dump_topology
								inline std::string Taskflow::dump_topologies() const {

									std::ostringstream os;

									for(const auto& tpg : _topologies) {
										tpg._dump(os);
									}

									return os.str();
								}

								// Function: dump
								inline void Taskflow::dump(std::ostream& os) const {

									os << "digraph Taskflow {\n";

									for(const auto& node : _graph) {
										node._dump(os);
									}

									os << "}\n";
								}

								// Function: dump
								// Dumps the taskflow in graphviz. The result can be viewed at http://www.webgraphviz.com/.
								inline std::string Taskflow::dump() const {
									std::ostringstream os;
									dump(os); 
									return os.str();
								}

};  // end of namespace tf. ---------------------------------------------------






#endif
CLOSE_NAMESPACE(Firestorm);
#endif
