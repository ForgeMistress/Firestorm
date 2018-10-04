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

template<class T>
struct member_traits
{
	static void copy(const T* from, T* to)
	{
		memcpy(to, from, sizeof(T));
	}

	static void make_new(void* data)
	{
		new (data) T();
	}
};

template<>
struct member_traits<String>
{
	static void copy(const String* from, String* to)
	{
		to->clear();
		to->resize(from->size());
		*to = *from;
	}

	static void make_new(void* data)
	{
		new (data) String();
	}
};

CLOSE_NAMESPACE(detail);

typedef void (*AllocFunc)(void*);

struct MemberDataBase
{
	MemberDataBase(size_t sizeOf, AllocFunc func)
		: SizeOf(sizeOf)
		, Func(func)
	{
	}

	size_t SizeOf;
	AllocFunc Func;
};

template<class T>
struct MemberData : public MemberDataBase
{
	MemberData()
		: MemberDataBase(sizeof(T), detail::member_traits<T>::make_new)
	{
	}
};

struct Buffer
{
	Buffer(){}
	~Buffer() { Free(); }

	inline void Alloc(size_t sizeInBytes)
	{
		FIRE_ASSERT(Data == nullptr);
		Data = (char*)malloc(sizeInBytes);
		Size = sizeInBytes;
	}

	inline void Init(size_t offset, AllocFunc& func)
	{
		func(Data + offset);
	}

	inline void Free()
	{
		FIRE_ASSERT(Data != nullptr);
		free((void*)Data);
		Data = nullptr;
	}


	void Set(int val, size_t size = std::numeric_limits<size_t>::max())
	{
		if(size == std::numeric_limits<size_t>::max())
		{
			size = Size;
		}
		memset((void*)Data, val, size);
	}

	template<class T>
	inline T* Get(size_t item) const
	{
		return reinterpret_cast<T*>(Data + (item * sizeof(T)));
	}

	template<class T>
	inline void Set(size_t item, const T* data)
	{
		T* dpos = reinterpret_cast<T*>(Data + (item * sizeof(T)));
		detail::member_traits<T>::copy(data, dpos);
	}

	/*
	FIRE_ASSERT(count > _data.n);

	Data newData;

	size_t bufferSize = component_type::SizeOf() * count;

	newData.buffer = (char*)malloc(bufferSize);
	memset(newData.buffer, 0, bufferSize);

	newData.n = _data.n;
	newData.capacity = count;
	newData.bufferSize = bufferSize;

	if(_data.bufferSize > 0)
	memcpy(newData.buffer, _data.buffer, _data.bufferSize);

	free(_data.buffer);
	_data = newData;
	*/

	// expands the size of the buffer by the provided factor.
	void Expand(uint16_t factor=2)
	{
		size_t newSize = Size * factor;

		// allocate the new buffer
		char* newData = (char*)malloc(newSize);

		// set the new buffer data.
		memset(newData, 0, newSize);

		// copy the old data over to the new buffer
		memcpy(newData, Data, Size);

		// release the old buffer
		free(Data);

		// set the buffer ptr to thhe new buffer
		Data = newData;

		// set thhe size to the new size.
		Size = newSize;
	}

	size_t Size; // the size of this particular buffer.
	char* Data{ nullptr };
};

OPEN_NAMESPACE(detail);

template<class... ArgsT> struct variadic_tpl_impl;

template<class ArgT, class... ArgsT>
struct variadic_tpl_impl<ArgT, ArgsT...>
{
	static void do_count(size_t& cnt)
	{
		variadic_tpl_impl<ArgsT...>::do_count(++cnt);
	}
};

template<class ArgT>
struct variadic_tpl_impl<ArgT>
{
	static void do_count(size_t& cnt)
	{
		++cnt;
	}
};

template<>
struct variadic_tpl_impl<>
{
	static void do_count(size_t& /* cnt */){}
};

CLOSE_NAMESPACE(detail);

template<class... ArgsT> struct variadic_tpl
{
	static size_t count()
	{
		size_t cnt;
		detail::variadic_tpl_impl<ArgsT...>::do_count(cnt);
		return cnt;
	}
};

struct ComponentManagerBase_MemoryManagement
{
	inline ComponentManagerBase_MemoryManagement(std::initializer_list<MemberDataBase>&& members, size_t initialCapacity = 0)
		: _count(0)
	{
		size_t NumMembers = members.size();
		_buffers.reserve(NumMembers);
		_buffers.resize(NumMembers);
		_memberData.reserve(NumMembers);
		_memberData.assign(members.begin(), members.end());
		if(initialCapacity > 0)
		{
			size_t i = 0;
			for(auto member = members.begin(); member != members.end(); ++member, ++i)
			{
				_buffers[i].Alloc(member->SizeOf * initialCapacity);
			}
		}
	}
	
	virtual ~ComponentManagerBase_MemoryManagement(){} // buffers free'd on destructor

	inline size_t MakeInstance()
	{
		size_t inst = _count;
		for(size_t i = 0; i < _buffers.size(); ++i)
		{
			_buffers[i].Init(inst * _memberData[i].SizeOf, _memberData[i].Func);
		}
		++_count;
		return inst;
	}

	inline void Alloc(size_t numberOfComponents)
	{
		for(size_t i = 0; i < _buffers.size(); ++i)
		{
			_buffers[i].Alloc(numberOfComponents * _memberData[i].SizeOf);
		}
	}

	inline void Expand(size_t numberOfMoreComponents)
	{
		for(size_t i = 0; i < _buffers.size(); ++i)
		{
			_buffers[i].Expand(numberOfMoreComponents * _memberData[i].SizeOf);
		}
	}

	template<class T>
	inline const T& Get(size_t row, size_t column) const
	{
		FIRE_ASSERT(row < _buffers.size());
		return &_buffers[row].Get<T>(column);
	}

	template<class T>
	inline T& Get(size_t row, size_t column)
	{
		FIRE_ASSERT(row < _buffers.size());
		return *_buffers[row].Get<T>(column);
	}

	template<class T>
	inline void Set(size_t row, size_t column, const T& data)
	{
		FIRE_ASSERT(row < _buffers.size());
		return _buffers[row].Set(column, &data);
	}

	Vector<Buffer>         _buffers;
	Vector<MemberDataBase> _memberData;
	size_t _count;
};

OPEN_NAMESPACE(detail);

struct member_data
{
	// the size of the actual member.
	size_t Size;
	size_t OffsetTo;
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

struct Component
{
	uint32_t Index;
};

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
		detail::member_traits<T>::copy(GetMemberPtr<T>(row, column), &output);
		return output;
	}

	/**
		Set the data with a copy operation.
	 **/
	template<class T>
	void SetData(size_t row, size_t column, const T& inData)
	{
		T* myDataPtr = GetMemberPtr<T>(row, column);
		detail::member_traits<T>::copy(&inData, myDataPtr);
	}

	template<class T>
	const T* GetMemberPtr(size_t row, size_t column) const
	{
		size_t memberPos = GetMemberPos(row, column);
		char* data = _data.buffer + memberPos;
		return reinterpret_cast<const T*>(data);
	}

	template<class T>
	T* GetMemberPtr(size_t row, size_t column)
	{
		size_t memberPos = GetMemberPos(row, column);
		char* data = _data.buffer + memberPos;
		return reinterpret_cast<T*>(data);
	}

	Component AddNewInstance()
	{
		if(_data.n == _data.capacity)
		{
			Alloc(_data.capacity * 2);
		}
		return Component{ ++_data.n };
	}

	Vector<Component> AddNewInstances(size_t numNewInstances)
	{
		Vector<Component> output;
		output.reserve(numNewInstances);

		for(size_t i=0; i<numNewInstances; ++i)
		{
			output.push_back(AddNewInstance());
		}

		return output;
	}

	/**
		Retrieve how many live objects are within the manager.
	 **/
	size_t GetCount() const
	{
		return _data.n;
	}
	
	/**
		Retrieve how much space is allocated for how many components. This is expressed as the number of components
		this manager can hold.
	 **/
	size_t GetCapacity() const
	{
		return _data.capacity;
	}

	/**
		What it says on the tin...
	 **/
	size_t GetBufferSizeInBytes() const
	{
		return _data.bufferSize;
	}

private:
	size_t GetMemberPos(size_t row, size_t column) const
	{
		auto& members = component_type::MyMembers();
		FIRE_ASSERT(column < members.size());

		auto& member = members[column];
		return GetRowStart(row) + member.OffsetTo;
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
		size_t capacity{ 0 };
		size_t bufferSize{ 0 };
		char*  buffer{ nullptr };
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

	size_t bufferSize = component_type::SizeOf() * count;

	newData.buffer = (char*)malloc(bufferSize);
	memset(newData.buffer, 0, bufferSize);

	newData.n = _data.n;
	newData.capacity = count;
	newData.bufferSize = bufferSize;

	if(_data.bufferSize > 0)
		memcpy(newData.buffer, _data.buffer, _data.bufferSize);

	free(_data.buffer);
	_data = newData;
}

CLOSE_NAMESPACE(Firestorm);
#endif
