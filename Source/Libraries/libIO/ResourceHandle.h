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
	Resource() {}

	// make an empty handle that's waiting for a resource.
	Resource(RefPtr<ResourceType>& obj, std::shared_future<LoadResult>& fut, const eastl::string& name)
	: _sFut(fut)
	, _name(name)
	, _obj(obj)
	, _hasFuture(true)
	{}

public:
	Resource(Resource<ResourceType>&& other)
	: _sFut(eastl::move(other._sFut))
	, _name(eastl::move(other._name))
	, _obj(eastl::move(other._obj))
	, _error(eastl::move(other._error))
	, _futurePulled(other._futurePulled)
	, _isFinished(other._isFinished)
	, _hasFuture(other._hasFuture)
	, _hasError(other._hasError)
	{}

	Resource(const Resource<ResourceType>& other)
	: _name(other._name)
	, _sFut(other._sFut)
	, _obj(other._obj)
	, _error(other._error)
	, _futurePulled(other._futurePulled)
	, _isFinished(other._isFinished)
	, _hasFuture(other._hasFuture)
	, _hasError(other._hasError)
	{
	}

	Resource& operator=(Resource<ResourceType>&& other)
	{
		if(this != &other)
		{
			_sFut = eastl::move(other._sFut);
			_name = eastl::move(other._name);
			_obj = eastl::move(other._obj);
			_error = eastl::move(other._error);
			_futurePulled = other._futurePulled;
			_isFinished = other._isFinished;
			_hasFuture = other._hasFuture;
			_hasError = other._hasError;
		}
		return *this;
	}

	Resource& operator=(const Resource<ResourceType>& other)
	{
		if(this != &other)
		{
			_sFut = other._sFut;
			_name = other._name;
			_obj = other._obj;
			_error = other._error;
			_futurePulled = other._futurePulled;
			_isFinished = other._isFinished;
			_hasFuture = other._hasFuture;
			_hasError = other._hasError;
		}
		return *this;
	}

	bool operator==(const Resource<ResourceType>& other)
	{
		return _name == other._name;
	}

	~Resource()
	{
		Release();
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

	eastl::shared_ptr<ResourceType> GetShared()
	{
		return _obj;
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
		return _hasFuture;
	}

	/**
		Retrieve whether or not this Resource has been passed through the loader and has some kind of state.
	**/
	bool IsFinished() const
	{
		if(!_isFinished)
		{
			try
			{
				if(!_futurePulled)
				{
					auto status = _sFut.wait_for(std::chrono::milliseconds(0));
					if(status == std::future_status::ready)
					{
						_isFinished = true;
						auto loadResult = _sFut.get();//eastl::get<1>(_tuppy).get();
						//_obj = loadResult.GetResource<ResourceType>();
						_error = loadResult.GetError();

						_futurePulled = true;
					}
				}
			}
			catch(std::exception& e)
			{
				FIRE_LOG_ERROR("Exception in Resource::IsFinished: %s", e.what());
			}
		}
		return _isFinished && _obj->IsReady();
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

private:
	void DoPull() const
	{
		// check the status of the future.
		auto status = _sFut.wait_for(std::chrono::milliseconds(0));
		if(status == std::future_status::ready)
		{
			_isFinished = true;
			auto loadResult = _sFut.get();//eastl::get<1>(_tuppy).get();
			_obj = loadResult.GetResource();
			_error = loadResult.GetError();

			_futurePulled = true;
		}
	}

	ResourcePtr PullData() const
	{
		if(_sFut.valid())
		{
			if(!_futurePulled)
			{
				DoPull();
			}
		}

		return _obj;
	}

	std::shared_future<LoadResult> _sFut;
	eastl::string _name;
	// pulled from future.
	mutable eastl::shared_ptr<ResourceType>    _obj;
	mutable Error                              _error;
	mutable bool                               _futurePulled{ false };
	mutable bool                               _isFinished{ false };

	bool _hasFuture{ false };
	bool _hasError{ false };
};

CLOSE_NAMESPACE(Firestorm);

#endif