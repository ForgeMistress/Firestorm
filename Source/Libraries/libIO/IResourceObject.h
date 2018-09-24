////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//<
//<  \file ResourceObject.h
//<
//<  Contains \ref IResourceObject
//<
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEOBJECT_H_
#define LIBIO_RESOURCEOBJECT_H_
#pragma once

#include <libCore/IRefCounted.h>
#include <json/value.h>

OPEN_NAMESPACE(Firestorm);

class IDocument;

class IResourceObject : public IRefCounted
{
	friend class ResourceHandle;
	friend class ResourceCache;
public:
	virtual ~IResourceObject() {}

private:
	size_t AddRef() { return ++_refCount; }
	size_t DelRef() { return --_refCount; }
	size_t GetRefCount() const { return _refCount; }

	size_t _refCount{ 0 };
};

CLOSE_NAMESPACE(Firestorm);
#endif