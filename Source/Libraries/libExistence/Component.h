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

class Entity;


OPEN_NAMESPACE(detail);

struct member_data
{
	// the size of the actual member.
	size_t Size;
	size_t Offset;
};

using member_data_list = Vector<member_data>;

template<class... MembersT> struct unroll_members;

template<class MemberT, class... MembersT>
struct unroll_members<MemberT, MembersT...>
{
	static void iterate(member_data_list& members, size_t& sizeOf, size_t& count, size_t prevOffset=0, size_t prevSize=0)
	{
		size_t size = sizeof(MemberT);
		sizeOf += size;
		++count;
		size_t memberOffset = prevOffset + prevSize;
		members.push_back(member_data{
			size,
			memberOffset
		});
		unroll_members<MembersT...>::iterate(members, sizeOf, count, memberOffset, size);
	}
};

// last field
template<class FieldT>
struct unroll_members<FieldT>
{
	static void iterate(member_data_list& sizes, size_t& sizeOf, size_t& count, size_t prevOffset=0, size_t prevSize=0)
	{
		size_t size = sizeof(FieldT);
		sizeOf += size;
		++count;
		size_t memberOffset = prevOffset + prevSize;
		sizes.push_back(member_data{
			size,
			memberOffset
		});
	}
};

// no more fields
template<>
struct unroll_members<>
{
	static void iterate(
		member_data_list& /* members    */,
		size_t&           /* sizeOf     */, 
		size_t&           /* count      */,
		size_t            /* prevOffset */,
		size_t            /* prevSize   */) {}
};

CLOSE_NAMESPACE(detail);

template<class... MembersT>
class IComponent
{
	// calculation flags.
	static bool _s_calculated;

	// static data for the IComponent.
	static size_t _s_sizeOf;
	static size_t _s_numMembers;
	static detail::member_data_list _s_members;

public:
	using base_component = IComponent<MembersT...>;

	/**
		Retrieve the number of fields this Component definition declares.
	 **/
	static size_t MyNumMembers()
	{
		if(!_s_calculated)
		{
			detail::unroll_members<MembersT...>::iterate(_s_members, _s_sizeOf, _s_numMembers);
			_s_calculated = true;
		}
		return _s_numMembers;
	}

	/**
		Retrieve the sizes of the individual fields.
	 **/
	static const detail::member_data_list& MyMembers()
	{
		if(!_s_calculated)
		{
			detail::unroll_members<MembersT...>::iterate(_s_members, _s_sizeOf, _s_numMembers);
			_s_members.shrink_to_fit();
			_s_calculated = true;
		}
		return _s_members;
	}

	/**
		Retrieve the size of the Component definition.
	 **/
	static size_t SizeOf()
	{
		if(!_s_calculated)
		{
			detail::unroll_members<MembersT...>::iterate(_s_members, _s_sizeOf, _s_numMembers);
			_s_calculated = true;
		}
		return _s_sizeOf;
	}
};

template<class... MembersT>
bool IComponent<MembersT...>::_s_calculated = false;

template<class... MembersT>
size_t IComponent<MembersT...>::_s_sizeOf = 0;

template<class... MembersT>
size_t IComponent<MembersT...>::_s_numMembers = 0;

template<class... MembersT>
detail::member_data_list IComponent<MembersT...>::_s_members;


/**

	Defining Components

 **/

/**
	This manager is a manager that you'd want to use when you know that the component
	passed to the template is pretty much going to be universal among all of the Entities
	that you're going to spawn. Things like position/rotation, names, etc...
 **/
template<class ComponentT>
class BasicComponentManager
{
	using component_type = typename ComponentT::base_component;
public:

	BasicComponentManager();
	virtual ~BasicComponentManager();

	/**
		Allocate space for the provided number of Components.
	 **/
	void Alloc(size_t count);

	/**
		Retrieve a copy of the data stored.
	 **/
	template<class T>
	T GetData(size_t row, size_t column) const
	{
		T output;
		memcpy(&output, GetMemberPtr<void>(row, column), GetMemberSize(column));
		return output;
	}

	template<>
	String GetData(size_t row, size_t column) const
	{
		const String* strPtr = GetMemberPtr<String>(row, column);
		return String(strPtr->begin(), strPtr->end());
	}

	/**
		Set the data with a copy operation.
	 **/
	template<class T>
	void SetData(size_t row, size_t column, const T& data);

	template<>
	void SetData(size_t row, size_t column, const String& data)
	{
		String* strPtr = GetMemberPtr<String>(row, column);
		strPtr->clear();
		strPtr->resize(data.size());
		*strPtr = data;
		//strPtr->assign(data.c_str());
	}

	template<class T>
	const T* GetMemberPtr(size_t row, size_t column) const
	{
		return reinterpret_cast<const T*>(
			reinterpret_cast<const char*>(_data.buffer)[GetMemberPos(row, column)]
		);
	}

	template<class T>
	T* GetMemberPtr(size_t row, size_t column)
	{
		size_t memberPos = GetMemberPos(row, column);
		return reinterpret_cast<T*>(
			reinterpret_cast<char*>(_data.buffer)[memberPos]
		);
	}

	/**
		Set the data with a move operation.
	 **/
	// template<class T>
	// void SetData(size_t row, size_t column, T&& data);

	/**
		Retrieve how many live objects are within the manager.
	 **/
	size_t GetCount() const;
	
	/**
		Retrieve how much space is allocated for how many components.
	 **/
	size_t GetCapacity() const;

	/**
		What it says on the tin...
	 **/
	size_t GetBufferSizeInBytes() const;

private:
	size_t GetMemberPos(size_t row, size_t column) const
	{
		auto& members = component_type::MyMembers();
		FIRE_ASSERT(column < members.size());

		auto& member = members[column];
		return GetComponentPos(row) + member.Offset;
	}

	// get the start of 
	size_t GetRowStart(size_t row) const
	{
		FIRE_ASSERT(row < _data.n);
		return component_type::SizeOf() * row;
	}

	size_t GetMemberSize(size_t column) const
	{
		auto& members = component_type::MyMembers();
		FIRE_ASSERT(column < members.size());
		return members[column].Size;
	}

	struct Data
	{
		size_t n{ 0 };
		size_t allocated{ 0 };
		size_t bufferSize{ 0 };
		void*  buffer{ nullptr };
	};
	Data _data;
};

template<class ComponentT>
BasicComponentManager<ComponentT>::BasicComponentManager()
{
}

template<class ComponentT>
BasicComponentManager<ComponentT>::~BasicComponentManager()
{
	if(_data.buffer)
		free(_data.buffer);
}

template<class ComponentT>
void BasicComponentManager<ComponentT>::Alloc(size_t count)
{
	FIRE_ASSERT(count > _data.n);

	Data newData;

	newData.buffer = malloc(component_type::SizeOf() * count);
	newData.n = _data.n;
	newData.allocated = count;
	newData.bufferSize = component_type::SizeOf() * count;

	free(_data.buffer);
	_data = newData;
}

template<class ComponentT>
template<class T>
void BasicComponentManager<ComponentT>::SetData(size_t row, size_t column, const T& tdata)
{
	auto& members = component_type::MyMembers();
	FIRE_ASSERT(column < members.size());

	auto& member = members[column];
	FIRE_ASSERT(member.Size == sizeof(tdata));

	size_t destinationStart = component_type::SizeOf() * row;
	size_t memberPos = destinationStart + member.Offset;
	//T* data = (T*)_data.buffer[memberPos];
	T* data = GetMemberPtr<T>(row, column);
	memcpy(data, &tdata, sizeof(tdata));
}

template<class ComponentT>
size_t BasicComponentManager<ComponentT>::GetCount() const
{
	return _data.n;
}

template<class ComponentT>
size_t BasicComponentManager<ComponentT>::GetCapacity() const
{
	return _data.allocated;
}

/**
	A component is an object that contains data and is a part of an Entity. Components should not
	contain application logic within their class definitions.
 **/
/*class IComponent
{
	FIRE_MIRROR_DECLARE(IComponent);
public:
	IComponent();
	virtual ~IComponent();
};*/

CLOSE_NAMESPACE(Firestorm);
#endif
