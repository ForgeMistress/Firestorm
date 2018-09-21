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

ResourceReference::ResourceReference()
{
}

ResourceReference::~ResourceReference()
{
}

const String& ResourceReference::GetResourcePath() const
{
	return _resourcePath;
}

bool ResourceReference::GetIsLoaded() const
{
	return _future.valid();
}

bool ResourceReference::HasError()
{
	return _future.valid() && _future.get().has_value();
}

bool ResourceReference::HasResource()
{
	return _future.valid() && _future.get().has_value();
}

RefPtr<IResourceObject> ResourceReference::GetResource()
{
	auto fvalue = _future.get();
	return fvalue.value();
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