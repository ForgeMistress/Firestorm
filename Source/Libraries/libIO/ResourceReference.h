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

OPEN_NAMESPACE(Firestorm);

/**
	\class ResourceReference

	This class instance does not contain any data. Instead, it merely defines a reference to a resource.
 **/
class ResourceReference : public Mirror::Object
{
	FIRE_MIRROR_DECLARE(ResourceReference, Mirror::Object);
public:
	ResourceReference();
	explicit ResourceReference(const String& resourcePath);
	virtual ~ResourceReference();

private:
	String _resourcePath;
};

CLOSE_NAMESPACE(Firestorm);

#endif