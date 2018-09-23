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

FIRE_ERRORCODE_DEF(ResourceReference::Errors::UNKNOWN_STATE, "the error state is in a state of limbo...");
FIRE_ERRORCODE_DEF(ResourceReference::Errors::THERE_IS_NO_ERROR, "there is no error to return");
FIRE_ERRORCODE_DEF(ResourceReference::Errors::NOT_FINISHED_LOADING, "the resource is not finished loading yet");

ResourceReference::ResourceReference(const String& path)
: _resourcePath(path)
, _error(Errors::UNKNOWN_STATE, "the error code is in an unknown state")
{
}

ResourceReference::ResourceReference(ResourceReference&& other)
: _resource(std::move(other._resource))
, _error(std::move(other._error))
, _errorSet(other._errorSet)
, _isReady(other._isReady)
, _resourcePath(std::move(other._resourcePath))
{
	other._resource = nullptr;
}

ResourceReference::~ResourceReference()
{
}

const String& ResourceReference::GetResourcePath() const
{
	return _resourcePath;
}

String ResourceReference::GetPathTo() const
{
	auto split = SplitString(_resourcePath, '/');
	split.pop_back();
	String out;
	for (auto str : split)
		out += str + "/";
	return out;
}

bool ResourceReference::IsReady() const
{
	return _isReady;
}

bool ResourceReference::HasError() const
{
	if(!IsReady())
	{
		// if we're not ready yet and someone attempts to call HasError, we should set the internal
		// error to NOT_FINISHED_LOADING for when they inevitably call GetError.
		// this way they can properly handle their code.
		_error = Error(Errors::NOT_FINISHED_LOADING, "you called ResourceReference::HasError "
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
				return Error(Errors::THERE_IS_NO_ERROR, "there was no error to report. the resource is set and ready to go.");
		}
	}
	return Error(Errors::UNKNOWN_STATE, "no error to retrieve just yet");
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

void ResourceReference::SetResult(const ResourceLoader::LoadResult& result)
{
	std::scoped_lock<Mutex> lock(_lock);
	if(result.has_value())
	{
		_resource = result.value();
	}
	else
	{
		_error = result.error();
		_errorSet = true;
	}
	_isReady = true;
}

CLOSE_NAMESPACE(Firestorm);