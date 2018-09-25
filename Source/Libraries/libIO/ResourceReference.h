///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceReference.h
//
//  Contains \ref ResourceReference.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEREFERENCE_H_
#define LIBIO_RESOURCEREFERENCE_H_
#pragma once

#include "IResourceObject.h"
#include "ResourceLoader.h"
#include "ResourceCache.h"
#include <libCore/IRefCounted.h>
#include <libMirror/Object.h>

OPEN_NAMESPACE(Firestorm);

/**
	\class ResourceReference

	This class instance does not contain any data. Instead, it merely defines a reference to a resource
	that can be passed into the ResourceMgr.
 **/
class ResourceReference final : public Mirror::Object
{
	FIRE_MIRROR_DECLARE(ResourceReference, Mirror::Object);
public:
	ResourceReference(const String& path = "");
	virtual ~ResourceReference();

	/**
		Retrieve the path to the resource on disk.
	 **/
	const String& GetResourcePath() const;

	/**
		Retrieve the path to this resource without the filename.
	 **/
	String GetPathTo() const;

private:
	friend class ResourceMgr;
	void SetResourcePath(const String& path);

	String _resourcePath;
};

CLOSE_NAMESPACE(Firestorm);

#endif