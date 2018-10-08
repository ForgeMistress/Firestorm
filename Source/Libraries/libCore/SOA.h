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
#include <libCore/TupleUtils.h>

#include "Memtraits.h"
#include <utility>
#include <tuple>

OPEN_NAMESPACE(Firestorm);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T, class... Types, size_t N = 0, std::size_t NumElements=sizeof...(Types)>
static constexpr void DoCalculateSizeof(T t, Types... tt, std::size_t& size)
{
	if constexpr(N == NumElements)
	{
		return;
	}
	else
	{
		size += sizeof(decltype(std::get<N>(types...)));
		DoCalculateSizeof<N + 1>(Types...);
	}
}

template<class... Types>
struct CalculateSizeOf
{
	template<std::size_t Index, typename tuple_type>
	static constexpr auto DoOp(tuple_type& tuple, std::size_t& result)
	{
		if constexpr(Index < sizeof...(Types))
		{
			result += sizeof(
				std::remove_pointer_t<
					std::decay_t<
						decltype(std::get<Index>(tuple))
					>
				>);
			DoOp<Index + 1>(tuple, result);
		}
	}

	template<class tuple_type>
	static constexpr std::size_t Op(tuple_type& tuple)
	{
		std::size_t out = 0;
		DoOp<0>(tuple, out);
		return out;
	}
};


template <char... Digits>
constexpr std::size_t parse()
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

template<std::size_t I>
struct index {};

template <char... Digits>
decltype(auto) operator"" _soa()
{
	return std::integral_constant<std::size_t, parse<Digits...>()>{};
}


enum struct AllocationRule : uint8_t
{
	FIXED_SIZE,   // the structure can not be resized automatically.
	VARIABLE_SIZE // the structure is allowed to reallocate its buffer size automatically.
};
template<class T> using raw = std::remove_pointer_t<std::remove_reference_t<T>>;

template<class... ItemsT>
struct SOA final : public std::tuple<std::add_pointer_t<raw<ItemsT>>...>
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// SOABase
	using tuple_type = std::tuple<std::add_pointer_t<raw<ItemsT>>...>;
	static constexpr std::size_t LLength = sizeof...(ItemsT); // number of individual members.
	static constexpr std::size_t Sizeof = CalculateSizeOf<raw<ItemsT>...>::Op(std::tuple<raw<ItemsT>...>());
		//std::tuple_size<std::tuple<raw<ItemsT>...>>::value;

	size_t _size{ 0 };
	size_t _capacity{ 0 };
	void* _buffer{ nullptr };
	tuple_type& _tuple;// { tuple_type() };

	template<
		size_t Index = 0, // start iteration at 0 index
		typename TCallable // the callable to be invoked for each tuple item
	>
	constexpr void IterateMembers(TCallable&& callable)
	{
		if constexpr(Index < LLength)
		{
			callable(Index, std::get<Index>(_tuple));

			if constexpr(Index + 1 < LLength)
			{
				IterateMembers<Index + 1>(callable);
			}
		}
	}

	template<
		std::size_t I,
		typename ElemT = std::remove_pointer_t<
			std::tuple_element_t<
				I,
				tuple_type
			>
		>
	>
	const ElemT* operator[](std::integral_constant<std::size_t,I>) const
	{
		return std::get<I>(_tuple);
	}

public:
	SOA():_tuple(*this)
	{
	}

	/**
		\brief Destructor.

		The destructor will free all of the memory that has been allocated. No action is required on your part
		when the time comes for this object to meet its untimely demise. Why are you a do horrible of a person to
		just let it die like that?

		\note You monster.
	**/
	~SOA()
	{
		Delete();
		Free();
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
		\brief Retrieve the capacity of the container.

		This one I will explain. Capacity refers to how much space for how many elements this ComponentDefinition
		can hold. Capacity is not related to Size in any way other than for bounds checking.
	**/
	size_t Capacity() const
	{
		return _capacity;
	}

	/**
		\brief Increment the size value of the container.

		This operation is provided as a means to increment the Size of the container when performing
		your own custom resizing operations. This operation includes an asserting bounds check
		that will fire off if \code{.cpp} Size >= Capacity \endcode.

		\return \c newSize The new size of the container.

		\post #ComponentDefinition::Size() += 1 || you fucked up and it fired off an assert.
	 **/
	inline size_t AddSize()
	{
		FIRE_ASSERT_MSG(_size < _capacity,
			"the capacity of the container has been reached. :(");
		++_size;
		return _size;
	}

	/**
		\brief Decrement the size value of the container.

		This operation is provided as an easy means to decrement the Size of the container. It will prevent
		the unsigned wrapping when Size == 0. You're welcome.

		\return \c newSize The new size of the container.

		\post #ComponentDefinition::Size() -= 1 || 0
	 **/
	inline size_t DecSize()
	{
		if(_size != 0)
			--_size; // unsigned value wrapping is a bitch...
		return _size;
	}

	/**
		\brief Allocate space in each of the internal member buffers.

		This operation will completely nuke all of the existing buffers and replace them with new buffers
		that hold enough space for \c numMembers elements.

		\arg \c numMembers The number of members you wish to allocate space for.

		\pre Size == #ComponentDefinition::Size()
		\pre Capacity == #ComponentDefinition::Capacity()
		\post Size == 0
		\post Capacity == numMembers

		\note This function is safe to call in the event you need to resize the buffer. The operation
		will take the contents of the old buffer and store a copy of it in the new one, so the data will
		remain.
	 **/
	void Alloc(size_t numMembers)
	{	
		FIRE_ASSERT_MSG(numMembers > _capacity, "the reallocated buffer must be larger than the previous one");
		
		// we'll store this properly as void* later. referenced as char* for now to make offset
		// calculation a bit cleaner to look at (void* can't reliably have pointer arithmetic applied 
		// to it on all compilers)]
		size_t bufferSize = Sizeof * numMembers;
		char* newBuffer = new char[bufferSize];
		memset(newBuffer, 0, bufferSize);

		// calculate the block offsets and store them in the tuple //
		char* nextElement = newBuffer;
		IterateMembers([this, &nextElement, &numMembers](size_t index, auto& memberPointer) {
			memberPointer = reinterpret_cast<decltype(memberPointer)>(nextElement);

			// once we've stored nextBlock in the tuple, we simply need to move the nextElement pointer
			// to the location of the next block.
			size_t sizeofElement = sizeof(raw<decltype(memberPointer)>);
			size_t sizeofBlock = sizeofElement * numMembers;
			nextElement += sizeofBlock;
		});

		if(_buffer)
		{
			// no sense in copying the entire old buffer, especially if _size is considerably less than the capacity.
			memcpy(newBuffer, _buffer, Sizeof * _size);
			Delete();
			Free();
		}
		_buffer = (void*)newBuffer;
		_capacity = numMembers;
		_size = 0;
	}
	/**
		Instantiate a new item. This will iterate all of the buffers and allocate a new instance of the fields
		for that item.

		\return \c itemIndex The index where this new item lives.

		\note This version of the function will manage the internal size count for you, so no further action is needed.
	 **/
	inline size_t New()
	{
		FIRE_ASSERT(_size < _capacity);

		IterateMembers([this](size_t index, auto& bufferPointer) {
			Mem::New<raw<decltype(bufferPointer)>>(bufferPointer, _size);
		});
		size_t itemIndex = _size;
		AddSize();
		return itemIndex;
	}

	/**
		Instantiate a bundle of new items. This will iterate all of the buffers and allocate a new instance of the fields
		for that item.

		\return \c itemIndex The index where this new item lives.

		\note This version of the function will manage the internal size count for you, so no further action is needed.
	**/
	inline Vector<size_t> BatchNew(size_t numItems)
	{
		FIRE_ASSERT(_size + (numItems-1) < _capacity);
		Vector<size_t> items(numItems);
		for(size_t i=0; i<numItems; ++i)
		{
			IterateMembers([this](size_t index, auto& bufferPointer) {
				Mem::New<raw<decltype(bufferPointer)>>(bufferPointer, _size);
			});
			items.push_back(_size);
			_size++;
		}
		return items;
	}

	/**
		Allocate a new instance for only the specified field.

		\return \c itemIndex The index where the new item lives.

		\warning THIS DOES NOT MODIFY THE INTERNAL SIZE COUNT.
		YOU HAVE TO MANAGE THE SIZE ON YOUR OWN IF YOU USE THIS!
	 **/
	template<
		size_t MemberIndex,
		class... Args,
		typename ElemT = std::remove_pointer_t<
			std::tuple_element_t<
				MemberIndex,
				tuple_type
			>
		>
	>
	inline size_t New()
	{
		FIRE_ASSERT_MSG(_size < _capacity, "reached max capacity. i gave you everything, and this is how you thank me?");
		Mem::New<ElemT>(std::get<MemberIndex>(_tuple), _size);
		return _size;
	}

	/**
		Delete the component at the specified index. This will iterate over all of the internal buffers and delete the
		requested item.

		\return \c opResults The operation will return a vector of operation results. 
		For each member that this definition defines, \c opResults.first is the index of the element that was "deleted,"
		(I.E., the instanceIndex you passed in), and \c opResults.second is the old index of the element that moved to take
		its place. These results can be used to perform any index remapping that you need to perform in your client classes.

		\note This version of the function will manage the internal size count for you, so no further action is needed.
	 **/
	using OpResults = std::pair<size_t, size_t>;
	inline std::pair<size_t, Vector<OpResults>> Delete(size_t instanceIndex)
	{
		Vector<OpResults> results;
		results.reserve(LLength);
		IterateMembers([this, &results](size_t index, auto& buffer) {
			results.push_back(Mem::Delete<raw<decltype(buffer)>>(&buffer, instanceIndex, _size));
		});
		DecSize();
		return std::make_pair(_size, std::move(results));
	}

	/**
		Delete the instance at the specified index for only the field that you provide.

		\warning THIS DOES NOT MODIFY THE INTERNAL SIZE COUNT.
		YOU HAVE TO MANAGE THE SIZE ON YOUR OWN IF YOU USE THIS!
	 **/
	template<
		size_t MemberIndex,
		typename ElemT = std::remove_pointer_t<
			std::tuple_element_t<
				MemberIndex,
				tuple_type
			>
		>
	>
	inline OpResults Delete(size_t instanceIndex)
	{
		return Mem::Delete<ElemT>(std::get<MemberIndex>(_tuple), instanceIndex, _size);
	}

	/**
		\brief Delete all of the members in the buffers.

		This function will iterate over all instances of all of the Member types and perform a delete operation.
		This will respect destruction logic in its members and so is the safest way to clear a buffer to
		make room for new instances of elements.
	 **/
	inline void Delete()
	{
		IterateMembers([this](size_t index, auto& buffer) {
			Mem::Delete<raw<decltype(buffer)>>(buffer, _size);
		});
		_size = 0;
	}

	/**
		Perform a wholesale 'free()' operation on the element buffer.

		\warning BE VERY CAREFUL WITH THIS! THIS OPERATION WILL PERFORM A COMPLETE FREE ON THE BUFFER WITHOUT
		ANY REGARD FOR DESTRUCTORS OR OTHER LOGIC THAT HAS TO RUN AS A RESULT OF OOBJECT DELETION.
		MAKE SURE THAT YOU KNOW WHAT YOU ARE DOING IF YOU CALL THIS FUNCTION! YOU. HAVE. BEEN. WARNED.
	**/
	inline void Free()
	{
		delete _buffer;
		_buffer = nullptr;
		_size = 0;
		_capacity = 0;
	}

	/**
		Retrieve a reference to the specified member at the specified instance index.
	 **/
	template<
		size_t MemberIndex,
		typename ElemT = std::remove_pointer_t<
			std::tuple_element_t<
				MemberIndex,
				tuple_type
			>
		>
	>
	inline ElemT& Get(size_t instanceIndex) const
	{
		/*FIRE_ASSERT_MSG(instanceIndex < _size,
			Format("the instance index was outside the bounds of the member buffer (Size = %d, Capacity = %d)",
			_size,
			_capacity));*/
		//return SOAB<ElemT>{std::get<MemberIndex>(_tuple)};
		return Mem::Get<ElemT>(std::get<MemberIndex>(_tuple), instanceIndex);
	}

	/**
		Set the value of thhe instance at the specified instance index to the provided value.
	 **/
	template<
		size_t MemberIndex,
		class Arg,
		typename ElemT = std::remove_pointer_t<
			std::tuple_element_t<
				MemberIndex,
				tuple_type
			>
		>
	>
	inline void Set(size_t instanceIndex, const Arg& value)
	{
		FIRE_ASSERT_MSG(instanceIndex < _capacity,
			Format("the instance index was outside the bounds of the member buffer (Index = %d, Size = %d, Capacity = %d)",
			instanceIndex,
			_size,
			_capacity));
		Mem::Set<ElemT>(std::get<MemberIndex>(_tuple), instanceIndex, value);
	}
};

// template<class... ItemsT>
// struct SOA : public SOABase<ItemsT...>
// {	
// 	using soa_type = SOA<ItemsT...>;
// 
// private:
// 	BufferInfo _info;
// 
// public:
// 	SOA()
// 	{
// 	}
// 
// 	/**
// 		\brief Destructor.
// 
// 		The destructor will free all of the memory that has been allocated. No action is required on your part
// 		when the time comes for this object to meet its untimely demise. Why are you a do horrible of a person to
// 		just let it die like that?
// 
// 		\note You monster.
// 	**/
// 	virtual ~SOA()
// 	{
// 		Delete();
// 		Free();
// 	}
// 
// 	/**
// 		\brief Retrieve the size of the container.
// 
// 		Do I really need to explain this one?
// 	**/
// 	size_t Size() const
// 	{
// 		return _size;
// 	}
// 
// 	/**
// 		\brief Retrieve the capacity of the container.
// 
// 		This one I will explain. Capacity refers to how much space for how many elements this ComponentDefinition
// 		can hold. Capacity is not related to Size in any way other than for bounds checking.
// 	**/
// 	size_t Capacity() const
// 	{
// 		return _capacity;
// 	}
// 
// 	/**
// 		\brief Increment the size value of the container.
// 
// 		This operation is provided as a means to increment the Size of the container when performing
// 		your own custom resizing operations. This operation includes an asserting bounds check
// 		that will fire off if \code{.cpp} Size >= Capacity \endcode.
// 
// 		\return \c newSize The new size of the container.
// 
// 		\post #ComponentDefinition::Size() += 1 || you fucked up and it fired off an assert.
// 	 **/
// 	inline size_t AddSize()
// 	{
// 		FIRE_ASSERT_MSG(_size < _capacity,
// 			"the capacity of the container has been reached. :(");
// 		++_size;
// 		return _size;
// 	}
// 
// 	/**
// 		\brief Decrement the size value of the container.
// 
// 		This operation is provided as an easy means to decrement the Size of the container. It will prevent
// 		the unsigned wrapping when Size == 0. You're welcome.
// 
// 		\return \c newSize The new size of the container.
// 
// 		\post #ComponentDefinition::Size() -= 1 || 0
// 	 **/
// 	inline size_t DecSize()
// 	{
// 		if(_size != 0)
// 			--_size; // unsigned value wrapping is a bitch...
// 		return _size;
// 	}
// 
// 	/**
// 		\brief Allocate space in each of the internal member buffers.
// 
// 		This operation will completely nuke all of the existing buffers and replace them with new buffers
// 		that hold enough space for \c numMembers elements.
// 
// 		\arg \c numMembers The number of members you wish to allocate space for.
// 
// 		\pre Size == #ComponentDefinition::Size()
// 		\pre Capacity == #ComponentDefinition::Capacity()
// 		\post Size == 0
// 		\post Capacity == numMembers
// 
// 		\note This function is safe to call in the event you need to resize the buffer. The operation
// 		will take the contents of the old buffer and store a copy of it in the new one, so the data will
// 		remain.
// 	 **/
// 	void Alloc(size_t numMembers)
// 	{	
// 		FIRE_ASSERT_MSG(numMembers > _capacity, "the reallocated buffer must be larger than the previous one");
// 		
// 		// we'll store this properly as void* later. referenced as char* for now to make offset
// 		// calculation a bit cleaner to look at (void* can't reliably have pointer arithmetic applied 
// 		// to it on all compilers)
// 		char* newBuffer = new char[Sizeof * numMembers];
// 
// 		// calculate the block offsets and store them in the tuple //
// 		char* nextElement = newBuffer;
// 		IterateMembers([this, &nextElement, &numMembers](size_t index, auto& memberPointer) {
// 			memberPointer = reinterpret_cast<decltype(memberPointer)>(nextElement);
// 
// 			// once we've stored nextBlock in the tuple, we simply need to move the nextElement pointer
// 			// to the location of the next block.
// 			size_t sizeofElement = sizeof(raw<decltype(memberPointer)>);
// 			size_t sizeofBlock = sizeofElement * numMembers;
// 			nextElement += sizeofBlock;
// 		});
// 
// 		if(_buffer)
// 		{
// 			// no sense in copying the entire old buffer, especially if _size is considerably less than the capacity.
// 			memcpy(newBuffer, _buffer, Sizeof * _size);
// 			Delete();
// 			Free();
// 		}
// 		_buffer = (void*)newBuffer;
// 		_capacity = numMembers;
// 		_size = 0;
// 	}
// 	/**
// 		Instantiate a new item. This will iterate all of the buffers and allocate a new instance of the fields
// 		for that item.
// 
// 		\return \c itemIndex The index where this new item lives.
// 
// 		\note This version of the function will manage the internal size count for you, so no further action is needed.
// 	 **/
// 	inline size_t New()
// 	{
// 		FIRE_ASSERT(_size < _capacity);
// 
// 		IterateMembers([this](size_t index, auto& bufferPointer) {
// 			Mem::New<raw<decltype(bufferPointer)>>(bufferPointer, _size);
// 		});
// 		size_t itemIndex = _size;
// 		AddSize();
// 		return itemIndex;
// 	}
// 
// 	/**
// 		Instantiate a bundle of new items. This will iterate all of the buffers and allocate a new instance of the fields
// 		for that item.
// 
// 		\return \c itemIndex The index where this new item lives.
// 
// 		\note This version of the function will manage the internal size count for you, so no further action is needed.
// 	**/
// 	inline Vector<size_t> BatchNew(size_t numItems)
// 	{
// 		FIRE_ASSERT(_size + (numItems-1) < _capacity);
// 		Vector<size_t> items(numItems);
// 		for(size_t i=0; i<numItems; ++i)
// 		{
// 			IterateMembers([this](size_t index, auto& bufferPointer) {
// 				Mem::New<raw<decltype(bufferPointer)>>(bufferPointer, _size);
// 			});
// 			items.push_back(_size);
// 			_size++;
// 		}
// 		return items;
// 	}
// 
// 	/**
// 		Allocate a new instance for only the specified field.
// 
// 		\return \c itemIndex The index where the new item lives.
// 
// 		\warning THIS DOES NOT MODIFY THE INTERNAL SIZE COUNT.
// 		YOU HAVE TO MANAGE THE SIZE ON YOUR OWN IF YOU USE THIS!
// 	 **/
// 	template<
// 		size_t MemberIndex,
// 		class... Args,
// 		typename ElemT = std::remove_pointer_t<
// 			std::tuple_element_t<
// 				MemberIndex,
// 				tuple_type
// 			>
// 		>
// 	>
// 	inline size_t New()
// 	{
// 		FIRE_ASSERT_MSG(_size < _capacity, "reached max capacity. i gave you everything, and this is how you thank me?");
// 		Mem::New<ElemT>(std::get<MemberIndex>(_this), _size);
// 		return _size;
// 	}
// 
// 	/**
// 		Delete the component at the specified index. This will iterate over all of the internal buffers and delete the
// 		requested item.
// 
// 		\return \c opResults The operation will return a vector of operation results. 
// 		For each member that this definition defines, \c opResults.first is the index of the element that was "deleted,"
// 		(I.E., the instanceIndex you passed in), and \c opResults.second is the old index of the element that moved to take
// 		its place. These results can be used to perform any index remapping that you need to perform in your client classes.
// 
// 		\note This version of the function will manage the internal size count for you, so no further action is needed.
// 	 **/
// 	using OpResults = std::pair<size_t, size_t>;
// 	inline std::pair<size_t, Vector<OpResults>> Delete(size_t instanceIndex)
// 	{
// 		Vector<OpResults> results;
// 		results.reserve(LLength);
// 		IterateMembers([this, &results](size_t index, auto& buffer) {
// 			results.push_back(Mem::Delete<raw<decltype(buffer)>>(&buffer, instanceIndex, _size));
// 		});
// 		DecSize();
// 		return std::make_pair(_size, std::move(results));
// 	}
// 
// 	/**
// 		Delete the instance at the specified index for only the field that you provide.
// 
// 		\warning THIS DOES NOT MODIFY THE INTERNAL SIZE COUNT.
// 		YOU HAVE TO MANAGE THE SIZE ON YOUR OWN IF YOU USE THIS!
// 	 **/
// 	template<
// 		size_t MemberIndex,
// 		typename ElemT = std::remove_pointer_t<
// 			std::tuple_element_t<
// 				MemberIndex,
// 				tuple_type
// 			>
// 		>
// 	>
// 	inline OpResults Delete(size_t instanceIndex)
// 	{
// 		return Mem::Delete<ElemT>(std::get<MemberIndex>(_this), instanceIndex, _size);
// 	}
// 
// 	/**
// 		\brief Delete all of the members in the buffers.
// 
// 		This function will iterate over all instances of all of the Member types and perform a delete operation.
// 		This will respect destruction logic in its members and so is the safest way to clear a buffer to
// 		make room for new instances of elements.
// 	 **/
// 	inline void Delete()
// 	{
// 		IterateMembers([this](size_t index, auto& buffer) {
// 			Mem::Delete<raw<decltype(buffer)>>(buffer, _size);
// 		});
// 		_size = 0;
// 	}
// 
// 	/**
// 		Perform a wholesale 'free()' operation on the element buffer.
// 
// 		\warning BE VERY CAREFUL WITH THIS! THIS OPERATION WILL PERFORM A COMPLETE FREE ON THE BUFFER WITHOUT
// 		ANY REGARD FOR DESTRUCTORS OR OTHER LOGIC THAT HAS TO RUN AS A RESULT OF OOBJECT DELETION.
// 		MAKE SURE THAT YOU KNOW WHAT YOU ARE DOING IF YOU CALL THIS FUNCTION! YOU. HAVE. BEEN. WARNED.
// 	**/
// 	inline void Free()
// 	{
// 		delete _buffer;
// 		_buffer = nullptr;
// 		_size = 0;
// 		_capacity = 0;
// 	}
// 
// 	/**
// 		Retrieve a reference to the specified member at the specified instance index.
// 	 **/
// 	template<
// 		size_t MemberIndex,
// 		typename ElemT = std::remove_pointer_t<
// 			std::tuple_element_t<
// 				MemberIndex,
// 				tuple_type
// 			>
// 		>
// 	>
// 	inline ElemT& Get(size_t instanceIndex) const
// 	{
// 		FIRE_ASSERT_MSG(instanceIndex < _size,
// 			Format("the instance index was outside the bounds of the member buffer (Size = %d, Capacity = %d)",
// 			_size,
// 			_capacity));
// 		return Mem::Get<ElemT>(std::get<MemberIndex>(_this), instanceIndex);
// 	}
// 
// 	/**
// 		Set the value of thhe instance at the specified instance index to the provided value.
// 	 **/
// 	template<
// 		size_t MemberIndex,
// 		class Arg,
// 		typename ElemT = std::remove_pointer_t<
// 			std::tuple_element_t<
// 				MemberIndex,
// 				tuple_type
// 			>
// 		>
// 	>
// 	inline void Set(size_t instanceIndex, const Arg& value)
// 	{
// 		FIRE_ASSERT_MSG(instanceIndex < _capacity,
// 			Format("the instance index was outside the bounds of the member buffer (Index = %d, Size = %d, Capacity = %d)",
// 			instanceIndex,
// 			_size,
// 			_capacity));
// 		Mem::Set<ElemT>(std::get<MemberIndex>(_this), instanceIndex, value);
// 	}
// };

CLOSE_NAMESPACE(Firestorm);
#endif
