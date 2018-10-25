///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Resource
//
//  A weak handle to a resource object.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEHANDLE_H_
#define LIBIO_RESOURCEHANDLE_H_
#pragma once

#include <libCore/RefPtr.h>
#include "ResourceLoader.h"
#include "IResourceObject.h"
#include <taskflow/taskflow.hpp>

OPEN_NAMESPACE(Firestorm);

struct ResourceHandleErrors
{
	FIRE_ERRORCODE(NULL_RESOURCE);
};

/**
	This object is returned by #ResourceMgr::QueueLoad. It is a wrapper around a pointer to a concrete resource type
	(the type specified by the template parameter) and is used to track the loading status of a particular 
	resource.

	It is used in two ways by the end user. The first way is as the object itself. It provides operations
	via the standard dot ( . ) operator that allows tracking. A common way to use this is during some
	update operation, you'd use the #Resource::IsFinished operation as follows.

	\code{.cpp}
	Resource<SomeType> resource = ResourceMgr.QueueLoad<SomeType>("some/file.bin");
	// busy wait while the resource isn't finished loading.
	while(true)
	{
		if(resource.IsFinished())
			break;
	}
	\endcode

	Once the resource is confirmed to be finished loading, the second method of usage can be used. This type also
	exposes the -> operator to retrieve a pointer to the underlying resource object. In this way, it acts as a kind
	of smart pointer and allows you to access methods of the underlying resource type.

	\code{.cpp}
	Resource<SomeType> resource = ResourceMgr.QueueLoad<SomeType>("some/file.bin");
	// busy wait while the resource isn't finished loading.
	while(true)
	{
		if(resource.IsFinished())
		break;
	}
	resource->Methods(); // invoke SomeType::Method()
	\endcode

	Once you are done with a particular resource, you can call Resource::Release to release the pointer. The system
	caches resources and handles the disposal of orphaned resources automatically (and the resource you received
	was probably even returned from that same cache if it was loaded before).

	\warning This type is move only. If you want to share a resource, you'll have to go through ResourceMgr::QueueLoad.
	It will go through the task graph, true, but the spawned tasks will check the cache and optimize the call. If a resource
	type depends on other resources (example, a meta-resource such as a pipeline or what have you that spawns other 
	resource loads) then the **primary** resource, should take in the Resource handle with an rvalue expression and you should
	use eastl::move to transfer it.
**/
template<class ResourceType>
class Resource final
{
	friend class ResourceMgr;
public:
	// make an empty handle that's waiting for a resource.
	Resource() {}

	Resource(ErrorCode* code, const eastl::string& name)
	: _name(name)
	, _obj(nullptr)
	, _hasError(true)
	{

	}

	Resource(RefPtr<ResourceType>& obj, const eastl::string& name)
	: _name(name)
	, _obj(obj)
	{}

public:
	// copy cast
	template<class U>
	Resource(const Resource<U>& other)
	: _name(other._name)
	, _obj(other._obj)
	, _error(other._error)
	, _hasError(other._hasError)
	{
		static_assert(eastl::is_base_of_v<ResourceType, U>, "invalid types");
	}

	// copy
	Resource(const Resource<ResourceType>& other)
	: _name(other._name)
	, _obj(other._obj)
	, _error(other._error)
	, _hasError(other._hasError)
	{
	}

	// move cast
	template<class U>
	Resource(Resource<U>&& other)
	: _name(eastl::move(other._name))
	, _obj(eastl::move(other._obj))
	, _error(eastl::move(other._error))
	, _hasError(other._hasError)
	{
		static_assert(eastl::is_base_of_v<ResourceType, U>, "invalid types");
	}

	// move
	Resource(Resource<ResourceType>&& other)
	: _name(eastl::move(other._name))
	, _obj(eastl::move(other._obj))
	, _error(eastl::move(other._error))
	, _hasError(other._hasError)
	{
	}

	// assign-move cast
	template<class U>
	Resource& operator=(Resource<U>&& other)
	{
		static_assert(eastl::is_base_of_v<ResourceType, U>, "invalid types");
		if(this != &other)
		{
			_name = eastl::move(other._name);
			_obj = eastl::move(other._obj);
			_error = eastl::move(other._error);
			_hasError = other._hasError;
		}
		return *this;
	}

	// assign-move
	Resource& operator=(Resource<ResourceType>&& other)
	{
		if(this != &other)
		{
			_name = eastl::move(other._name);
			_obj = eastl::move(other._obj);
			_error = eastl::move(other._error);
			_hasError = other._hasError;
		}
		return *this;
	}

	template<class U>
	Resource& operator=(const Resource<U>& other)
	{
		static_assert(eastl::is_base_of_v<ResourceType, U>, "invalid types");
		if(this != &other)
		{
			_name = other._name;
			_obj = other._obj;
			_error = other._error;
			_hasError = other._hasError;
		}
		return *this;
	}

	Resource& operator=(const Resource<ResourceType>& other)
	{
		if(this != &other)
		{
			_name = other._name;
			_obj = other._obj;
			_error = other._error;
			_hasError = other._hasError;
		}
		return *this;
	}

	template<class U>
	bool operator==(const Resource<U>& other)
	{
		static_assert(eastl::is_base_of_v<ResourceType, U>, "invalid types");
		return _name == other._name;
	}

	bool operator==(const Resource<ResourceType>& other)
	{
		return _name == other._name;
	}

	~Resource()
	{
		Release();
	}

	template<class T>
	T* get()
	{
		return static_cast<T*>(_obj.get());
	}

	template<class T>
	const T* get() const
	{
		return static_cast<T*>(_obj.get());
	}

	const ResourceType* get() const
	{
		return _obj.get();
	}

	ResourceType* get()
	{
		return _obj.get();
	}

	const ResourceType* operator->() const
	{
		return _obj.get();
	}

	ResourceType* operator->()
	{
		return _obj.get();
	}

	/**
		Retrieve the error.
	**/
	Error GetError() const
	{
		return _error;
	}

	/**
		Check whether or not this instance of the Resource is valid.
		\note A Resource is valid when the following conditions are met.
		- A Future object was passed to the instance either through an assignment or construction (or both).
	**/
	bool IsValid() const
	{
		return _obj != nullptr;
	}

	/**
		Retrieve whether or not this Resource has been passed through the loader and has some kind of state.
	**/
	bool IsFinished() const
	{
		return _obj->IsReady();
	}

	/**
		Retrieve whether or not the Resource is reporting an error.
	**/
	bool HasError() const
	{
		return _error.GetCode() != nullptr;
	}

	/**
		Release the resource from this handle as well as the active error.
	**/
	void Release()
	{
		_obj = nullptr;
		_error.Set(nullptr, "");
	}

	shared_ptr<ResourceType> GetShared()
	{
		return _obj;
	}

	// don't touch this
	string _name;
	// don't touch this
	shared_ptr<ResourceType> _obj{ nullptr };
	// don't touch this
	Error _error;
	// don't touch this
	bool _isFinished{ false };
	// don't touch this
	bool _hasError{ false };
};

CLOSE_NAMESPACE(Firestorm);

#endif