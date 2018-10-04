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

OPEN_NAMESPACE(Firestorm);


OPEN_NAMESPACE(detail);

template<class T>
struct add_ptr
{
	using type = T*;
};

CLOSE_NAMESPACE(detail);

template<class T>
struct MemberData
{
	MemberData()
	{
	}

	void Alloc(size_t numElements)
	{
		Free();
		Buffer = (T*)malloc(numElements * sizeof(T));
	}

	void Free()
	{
		if(Buffer)
		{
			free(Buffer);
			Buffer = nullptr;
		}
	}

	inline T* Get(size_t index)
	{
		return Buffer[index];
	}

	inline void Set(size_t index, const T* value)
	{
		memcpy(Buffer[index], value, sizeof(T));
	}

	T* Buffer{ nullptr };
};


template<class... MembersT>
struct ComponentDefinition : public std::tuple<MembersT...>
{
	using tuple_type = std::tuple<MembersT...>;

	template<int MemberIndex>
	inline auto _get_member_buffer()
	{
		return std::get<MemberIndex, tuple_type>(*this);
	}

	void Alloc(size_t numMembers)
	{

	}

	template<int MemberIndex, class T>
	T& Get(size_t instanceIndex)
	{
		return &_get_member_buffer<MemberIndex>().Get(instanceIndex);
	}

	template<int MemberIndex, class T>
	void Set(size_t instanceIndex, const T& value)
	{
		_get_member_buffer<MemberIndex>().Set(instanceIndex, &value);
	}
};



CLOSE_NAMESPACE(Firestorm);
#endif
