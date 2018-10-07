///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Component.h
//
//  A component in an entity that contains only data at the high level.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_COMPONENT_H_
#define LIBEXISTENCE_COMPONENT_H_
#pragma once

#include <libCore/libCore.h>
#include "Entity.h"
#include <libCore/Logger.h>
#include <libCore/TupleUtils.h>
#include "Memtraits.h"
#include <utility>
namespace Firestorm{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FIRE_BUFFER_TC(left, right, func)                                   \
static_assert(std::is_same<left, right>::value ||                           \
std::is_convertible<left,right>::value,                                     \
"Invalid type passed to '"func"'. The field types must be the _exact_ same");

#define tt_this ((tuple_type&)(*this))

/**
	Size and capacity data for every ComponentDefinition.
**/
struct BufferInfo
{
	size_t Size{ 0 };
	size_t Capacity{ 0 };
	void* Buffer{ nullptr };
};

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

template<class... MembersT>
struct ComponentDefinition : public std::tuple<std::add_pointer_t<std::remove_reference_t<MembersT>>...>
{
	using tuple_type = std::tuple<std::add_pointer_t<std::remove_reference_t<MembersT>>...>;
	static constexpr std::size_t LLength = sizeof...(MembersT); // number of individual members.
	template<class T> using raw = std::remove_pointer_t<std::remove_reference_t<T>>;

private:
	BufferInfo _info;
	static std::size_t Sizeof;

public:
	ComponentDefinition()
	{
		Sizeof = CalculateSizeOf<MembersT...>::Op(tt_this);
	}


	/**
		\brief Destructor.

		The destructor will free all of the memory that has been allocated. No action is required on your part
		when the time comes for this object to meet its untimely demise. Why are you a do horrible of a person to
		just let it die like that?

		\note You monster.
	**/
	virtual ~ComponentDefinition()
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
		return _info.Size;
	}

	/**
		\brief Retrieve the capacity of the container.

		This one I will explain. Capacity refers to how much space for how many elements this ComponentDefinition
		can hold. Capacity is not related to Size in any way other than for bounds checking.
	**/
	size_t Capacity() const
	{
		return _info.Capacity;
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
		FIRE_ASSERT_MSG(_info.Size < _info.Capacity,
			"the capacity of the container has been reached. :(");
		++_info.Size;
		return _info.Size;
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
		if(_info.Size != 0)
			--_info.Size; // unsigned value wrapping is a bitch...
		return _info.Size;
	}

	template<
		size_t Index = 0, // start iteration at 0 index
		typename TCallable // the callable to be invoked for each tuple item
	>
	constexpr void IterateMembers(TCallable& callable)
	{
		if constexpr(Index < LLength)
		{
			std::invoke(callable, Index, std::get<Index>(tt_this));

			if constexpr(Index + 1 < LLength)
			{
				IterateMembers<Index + 1>(callable);
			}
		}
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
		FIRE_ASSERT(numMembers > _info.Capacity);

		// we'll store this properly as void* later. referenced as char* for now to make offset
		// calculation a bit cleaner to look at (void* can't reliably have pointer arithmetic applied to it)
		char* newBuffer = new char[Sizeof * numMembers];

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

		if(_info.Buffer)
		{
			// no sense in copying the entire old buffer, especially if _info.Size is considerably less than the capacity.
			memcpy(newBuffer, _info.Buffer, Sizeof * _info.Size);
			Delete();
			Free();
		}
		_info.Buffer = (void*)newBuffer;
		_info.Capacity = numMembers;
		_info.Size = 0;
	}
	/**
		Instantiate a new item. This will iterate all of the buffers and allocate a new instance of the fields
		for that item.

		\return \c itemIndex The index where this new item lives.

		\note This version of the function will manage the internal size count for you, so no further action is needed.
	 **/
	size_t New()
	{
		FIRE_ASSERT(_info.Size < _info.Capacity);

		IterateMembers([this](size_t index, auto& bufferPointer) {
			Mem::New<raw<decltype(bufferPointer)>>(bufferPointer, _info.Size);
		});
		size_t itemIndex = _info.Size;
		AddSize();
		return itemIndex;
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
	size_t New()
	{
		FIRE_ASSERT_MSG(_info.Size < _info.Capacity, "reached max capacity. i gave you everything, and this is how you thank me?");
		Mem::New<ElemT>(std::get<MemberIndex>(tt_this), _info.Size);
		return _info.Size;
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
	std::pair<size_t, Vector<OpResults>> Delete(size_t instanceIndex)
	{
		Vector<OpResults> results;
		results.reserve(LLength);
		IterateMembers([this, &results](size_t index, auto& buffer) {
			results.push_back(Mem::Delete<raw<decltype(buffer)>>(&buffer, instanceIndex, _info.Size));
		});
		DecSize();
		return std::make_pair(_info.Size, std::move(results));
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
	OpResults Delete(size_t instanceIndex)
	{
		return Mem::Delete<ElemT>(std::get<MemberIndex>(tt_this), instanceIndex, _info.Size);
	}

	/**
		\brief Delete all of the members in the buffers.

		This function will iterate over all instances of all of the Member types and perform a delete operation.
		This will respect destruction logic in its members and so is the safest way to clear a buffer to
		make room for new instances of elements.
	 **/
	void Delete()
	{
		IterateMembers([this](size_t index, auto& buffer) {
			Mem::Delete<raw<decltype(buffer)>>(buffer, _info.Size);
		});
		_info.Size = 0;
	}

	/**
		Perform a wholesale 'free()' operation on the element buffer.

		\warning BE VERY CAREFUL WITH THIS! THIS OPERATION WILL PERFORM A COMPLETE FREE ON THE BUFFER WITHOUT
		ANY REGARD FOR DESTRUCTORS OR OTHER LOGIC THAT HAS TO RUN AS A RESULT OF OOBJECT DELETION.
		MAKE SURE THAT YOU KNOW WHAT YOU ARE DOING IF YOU CALL THIS FUNCTION! YOU. HAVE. BEEN. WARNED.
	**/
	void Free()
	{
		delete _info.Buffer;
		_info.Buffer = nullptr;
		_info.Size = 0;
		_info.Capacity = 0;
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
	ElemT& Get(size_t instanceIndex) const
	{
		FIRE_ASSERT_MSG(instanceIndex < _info.Size,
			Format("the instance index was outside the bounds of the member buffer (Size = %d, Capacity = %d)",
			_info.Size,
			_info.Capacity));
		return Mem::Get<ElemT>(std::get<MemberIndex>(tt_this), instanceIndex);
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
	void Set(size_t instanceIndex, const Arg& value)
	{
		FIRE_ASSERT_MSG(instanceIndex < _info.Capacity,
			Format("the instance index was outside the bounds of the member buffer (Index = %d, Size = %d, Capacity = %d)",
			instanceIndex,
			_info.Size,
			_info.Capacity));
		Mem::Set<ElemT>(std::get<MemberIndex>(tt_this), instanceIndex, value);
	}
};

template<class... MembersT>
std::size_t ComponentDefinition<MembersT...>::Sizeof = 0;

}
#endif
