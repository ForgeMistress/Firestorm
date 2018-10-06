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

#define FIRE_BUFFER_TC(left, right, func)                                 \
static_assert(std::is_same<left, right>::value,                           \
"Invalid type passed to '"func"'. The field types must be the _exact_ same.");

#define tt_this ((tuple_type&)(*this))

/**
	Size and capacity data for every ComponentDefinition.
**/
struct BufferInfo
{
	size_t Size{ 0 };
	size_t Capacity{ 0 };
	void* Buffer;
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
struct ComponentDefinition : public std::tuple<std::add_pointer_t<std::decay_t<MembersT>>...>
{
	using tuple_type = std::tuple<std::add_pointer_t<std::decay_t<MembersT>>...>;

	

	static constexpr std::size_t LLength = sizeof...(MembersT); // number of individual members.


	using def_type = ComponentDefinition<MembersT...>;

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
		/*tuple_for_each(tt_this, [this](auto& buffer) {
			Mem::Free<decltype(buffer)>(buffer);
		});*/
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
		typename TTuple,  // the tuple type
		size_t Size =
		std::tuple_size_v<
			std::remove_reference_t<TTuple>>, // tuple size
			typename TCallable // the callable to bo invoked for each tuple item
		>
		void my_for_each(TTuple&& tuple, TCallable&& callable, void** arg)
	{
		if constexpr (Index < Size)
		{
			invoke(callable, &std::get<Index>(tuple), arg);

			if constexpr (Index + 1 < Size)
				my_for_each<Index + 1>(
					std::forward<TTuple>(tuple),
					std::forward<TCallable>(callable),
					std::forward<void**>(arg));
		}
	}
#define get_tup(index, tup) std::get<index>(tup)
	template<class... Types>
	struct ForwardAddresses
	{
		template<std::size_t Index, typename tuple_type, typename CurrentPointerType, typename PrevPointerType>
		static constexpr auto DoOp(tuple_type& tuple, char** previous, std::size_t size)
		{
			constexpr PrevPointerType previousCasted = *reinterpret_cast<PrevPointerType>(&previous);
			constexpr CurrentPointerType currentCasted = 
				reinterpret_cast<CurrentPointerType>(std::get<Index>(tuple));
			if constexpr(Index == 0)
			{
				std::set<Index>(tuple, *previous);
				if constexpr(Index + 1 < sizeof...(Types))
				{
					DoOp<Index + 1, decltype(get_tup(Index + 1, tuple)), decltype(get_tup(Index, tuple))>(tuple, &(void*)currentCasted);
				}
			}
			else if constexpr(Index < sizeof...(Types))
			{
				std::set<Index>(tuple, *previous + size);
				DoOp<Index + 1, decltype(get_tup(Index + 1, tuple)), decltype(get_tup(Index, tuple))>(tuple, &(void*)currentCasted);
			}
		}

		template<class tuple_type>
		static constexpr void Op(tuple_type& tuple, char** first, std::size_t size)
		{
			DoOp<0, tuple_type, decltype(std::get<0>(tuple)), decltype(std::get<0>(tuple))>(tuple, first, size);
		}
	};

	/**
		\brief Allocate space in each of the internal member buffers.

		This operation will completely nuke all of the existing buffers and replace them with new buffers
		that hold enough space for \c numMembers elements.

		\arg \c numMembers The number of members you wish to allocate space for.

		\pre Size == #ComponentDefinition::Size()
		\pre Capacity == #ComponentDefinition::Capacity()
		\post Size == 0
		\post Capacity == numMembers
	 **/
	void Alloc(size_t numMembers)
	{
		_info.Capacity = numMembers;
		_info.Size = 0;

		/*tuple_for_each((tuple_type&)(*this), [numMembers](auto& buffer) {
			Mem::Free<decltype(buffer)>(buffer);
			buffer = Mem::Alloc<decltype(buffer)>(numMembers);
		});*/

		void* newBuffer = malloc(Sizeof * numMembers);
		ForwardAddresses<MembersT...>::Op(tt_this, &(char*)newBuffer, numMembers);
		/*tuple_for_each(tt_this, [this, numMembers, &newBuffer](auto& blockWereSetting, void** previousBlock) {
			using ptr = decltype(blockWereSetting);
			ptr* prevPtr = reinterpret_cast<ptr*>(*previousBlock);

			// the first iteration case.
			if(prevPtr == newBuffer)
				blockWereSetting = prevPtr;
			else
				blockWereSetting = prevPtr + numMembers;

			*previousBlock = blockWereSetting;
		}, nb);*/

		if(_info.Buffer)
		{
			free(_info.Buffer);
		}
		_info.Buffer = newBuffer;
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
		Instantiate a new item. This will iterate all of the buffers and allocate a new instance of the fields
		for that item.

		\return \c itemIndex The index where this new item lives.

		\note This version of the function will manage the internal size count for you, so no further action is needed.
	 **/
	template<
		template<class... MembersConstructorArgsPack>
		class... MembersPack
	>
	size_t New(MembersPack<MembersConstructorArgsPack&&...>&&... args)
	{
		/*static_assert(LLength == sizeof...(args),
			"initialization packs were not provided for all of the fields. this function is an all or nothing deal broheim.");
		FIRE_ASSERT_MSG(_info.Size < _info.Capacity, "out of memory. can't instantiate any more instances of the stuff.");

		size_t instanceIndex = _info.Size;
		constexpr size_t N = 0;
		if constexpr(N < LLength)
		{

		}

		// todo: make this work with only one loop.
		tuple_for_each(tt_this, [this, &instanceIndex](auto& bufferPtr) {
			Memtraits<decltype(bufferPtr)>::New(bufferPtr, instanceIndex);
		});

		tuple_for_each(tt_this, [this](auto& bufferPtr) {

		});

		//new_for_each((tuple_type&)(*this), [this](auto& buffer) {
			//Memtraits<decltype(buffer)>::New(&buffer, _info.Size, std::forward<Args>(args)...);
		//}, std::make_tuple(Args...));

		size_t out = _info.Size;
		AddSize();
		return out;*/
		return 0;
	}

	/**
		Allocate a new instance for only the specified field.

		\warning THIS DOES NOT MODIFY THE INTERNAL SIZE COUNT.
		YOU HAVE TO MANAGE THE SIZE ON YOUR OWN IF YOU USE THIS!
	 **/
	template<
		size_t MemberIndex,
		class T,
		class... Args,
		class TT = std::remove_pointer_t<std::decay<T>::type>,
		typename ElemT = std::remove_pointer_t<
			std::decay_t<
				std::tuple_element_t<
					MemberIndex,
					tuple_type
				>
			>
		>
	>
	void New(Args&&... args)
	{
		FIRE_BUFFER_TC(TT, ElemT, "New");
		FIRE_ASSERT_MSG(_info.Size < _info.Capacity, "reached max capacity. i gave you everything, and this is how you thank me?");
		Memtraits<ElemT>::New(std::get<MemberIndex>(tt_this), _info.Size, std::forward<Args>(args)...);
	}

	/**
		Delete the component at the specified index. This will iterate over all of the internal buffers and delete the
		requested item.

		\note This version of the function will manage the internal size count for you, so no further action is needed.
	 **/
	void Delete(size_t instanceIndex)
	{
		/*tuple_for_each(tt_type, [this](auto& buffer) {
			Memtraits<decltype(buffer)>::Delete(&buffer, instanceIndex, _info);
		});*/
		DecSize();
	}

	/**
		Delete the instance at the specified index for only the field that you provide.

		\warning THIS DOES NOT MODIFY THE INTERNAL SIZE COUNT.
		YOU HAVE TO MANAGE THE SIZE ON YOUR OWN IF YOU USE THIS!
	 **/
	template<
		size_t MemberIndex,
		class T,
		class... Args,
		class TT = std::remove_pointer_t<std::decay<T>::type>,
		typename ElemT = std::remove_pointer_t<
			std::decay_t<
				std::tuple_element_t<
					MemberIndex,
					tuple_type
				>
			>
		>
	>
	void Delete(size_t instanceIndex)
	{
		FIRE_BUFFER_TC(TT, ElemT, "Delete");
		Memtraits<TT>::Delete(std::get<MemberIndex>(tt_this), instanceIndex, _info);
	}

	/**
		Perform a wholesale 'free()' operation on the element buffer at the specified MemberIndex.

		\warning BE VERY CAREFUL WITH THIS! THIS OPERATION WILL PERFORM A COMPLETE FREE ON THE BUFFER
		SO MAKE SORE THAT YOU KNOW WHAT YOU ARE DOING IF YOU CALL THIS FUNCTION! YOU. HAVE. BEEN. WARNED.
	 **/
	template<
		size_t MemberIndex,
		class T,
		class... Args,
		class TT = std::remove_pointer_t<std::decay<T>::type>,
		typename ElemT = std::remove_pointer_t<
			std::decay_t<
				std::tuple_element_t<
					MemberIndex,
					tuple_type
				>
			>
		>
	>
	void Free()
	{
		FIRE_BUFFER_TC(TT, ElemT, "Free");
		Mem::Free<ElemT>(_info.Buffer, Capacity);
	}

	/**
		Perform a wholesale 'free()' on ALL of the member buffers.

		\warning YOU SHOULD KNOW WHAT THIS DOES! THIS WILL NUKE THE ENTIRE DAMN BUFFER! YOU KNOW
		WHAT YOU'RE GETTING YOURSELF INTO!
	 **/
	void Free()
	{
		/*tuple_for_each(tt_this, [this](auto& buffer) {
			Mem::Free<decltype(buffer)>(buffer);
		});*/
	}

	/**
		Retrieve a reference to the specified member at the specified instance index.
	 **/
	template<
		size_t MemberIndex,
		class T,
		class... Args,
		class TT = std::remove_pointer_t<std::decay<T>::type>,
		typename ElemT = std::remove_pointer_t<
			std::decay_t<
				std::tuple_element_t<
					MemberIndex,
					tuple_type
				>
			>
		>
	>
		T& Get(size_t instanceIndex) const
	{
		FIRE_BUFFER_TC(TT, ElemT, "Get");
		FIRE_ASSERT_MSG(instanceIndex < _info.Size,
			Format("the instance index was outside the bounds of the member buffer (Size = %d, Capacity = %d)",
			_info.Size,
			_info.Capacity));
		return Memtraits<TT>::Get(std::get<MemberIndex>(tt_this), instanceIndex);
	}

	/**
		Set the value of thhe instance at the specified instance index to the provided value.
	 **/
	template<
		size_t MemberIndex,
		class T,
		class... Args,
		class TT = std::remove_pointer_t<std::decay<T>::type>,
		typename ElemT = std::remove_pointer_t<
			std::decay_t<
				std::tuple_element_t<
					MemberIndex,
					tuple_type
				>
			>
		>
	>
		void Set(size_t instanceIndex, const TT& value)
	{
		FIRE_BUFFER_TC(TT, ElemT, "Set");
		FIRE_ASSERT_MSG(instanceIndex < _info.Size,
			Format("the instance index was outside the bounds of the member buffer (Size = %d, Capacity = %d)",
			_info.Size,
			_info.Capacity));
		// MemberBufferTraits::Copy<T>(std::get<MemberIndex>(*this), instanceIndex, &value);
	}
};



}
#endif
