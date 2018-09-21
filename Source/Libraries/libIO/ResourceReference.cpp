///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceReference.cpp
//
//  A reference to a resource. Contains an instance of File.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceReference.h"

OPEN_NAMESPACE(Firestorm);

FIRE_MIRROR_DEFINE(Firestorm::ResourceReference)
{
	Class
		.property("resourcePath", &ResourceReference::_resourcePath);
}

ErrorCode ResourceReference::Errors::UNKNOWN_STATE("the error state is in a state of limbo...");
ErrorCode ResourceReference::Errors::THERE_IS_NO_ERROR("there is no error to return");
ErrorCode ResourceReference::Errors::NOT_FINISHED_LOADING("the resource is not finished loading yet");

ResourceReference::ResourceReference(const String& path)
: _resourcePath(path)
, _error(&Errors::UNKNOWN_STATE, "the error code is in an unknown state")
{
}

ResourceReference::~ResourceReference()
{
}

const String& ResourceReference::GetResourcePath() const
{
	return _resourcePath;
}

bool ResourceReference::IsReady() const
{
	// return early if we're already ready.
	if(_isReady) return _isReady;

	std::future_status status = _future.wait_for(std::chrono::milliseconds(1));
	if(status == std::future_status::ready)
	{
		_isReady = true;
		auto result = _future.get();
		if(result.has_value())
		{
			_resource = result.value();
		}
		else
		{
			_error = result.error();
			_errorSet = true;
		}
	}
	return _isReady;
}

bool ResourceReference::HasError() const
{
	if(!IsReady())
	{
		// if we're not ready yet and someone attempts to call HasError, we should set the internal
		// error to NOT_FINISHED_LOADING for when they inevitably call GetError.
		// this way they can properly handle their code.
		_error = Error(&Errors::NOT_FINISHED_LOADING, "you called ResourceReference::HasError "
                                                      "a little early there brah...");
		return true;
	}
	return _errorSet;
}

bool ResourceReference::HasResource() const
{
	if(IsReady())
	{
		return _resource != nullptr;
	}
	return false;
}

RefPtr<IResourceObject> ResourceReference::GetResource() const
{
	if(IsReady())
	{
		return _resource;
	}
	return nullptr;
}

Error ResourceReference::GetError() const
{
	if(IsReady())
	{
		if(_errorSet)
		{
			return _error;
		}
		else
		{
			if(_resource)
				return Error(&Errors::THERE_IS_NO_ERROR, "there was no error to report. the resource is set and ready to go.");
		}
	}
	return Error(&Errors::UNKNOWN_STATE, "no error to retrieve just yet");
}

void ResourceReference::Release()
{
	if(_resource)
		_resource = nullptr;
}

void ResourceReference::SetResourcePath(const String& path)
{
	_resourcePath = path;
}

void ResourceReference::SetFuture(std::future<ResourceMgr::LoadResult>&& future)
{
	_future = std::move(future);
}

CLOSE_NAMESPACE(Firestorm);