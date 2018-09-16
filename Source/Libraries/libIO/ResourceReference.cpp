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
	Class.property("resourcePath", &ResourceReference::_resourcePath);
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
	return _isLoaded;
}

void ResourceReference::SetResult(const Result<void, Error>& result)
{
}

CLOSE_NAMESPACE(Firestorm);