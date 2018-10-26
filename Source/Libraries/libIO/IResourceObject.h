////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//<
//<  \file ResourceObject.h
//<
//<  Contains \ref IResourceObject
//<
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEOBJECT_H_
#define LIBIO_RESOURCEOBJECT_H_
#pragma once

#include <libCore/RefPtr.h>
#include <libCore/MirrorMacros.h>

OPEN_NAMESPACE(Firestorm);

class IResourceObject
{
	FIRE_MIRROR_DECLARE(IResourceObject);
public:
	struct ILoadOp
	{
		virtual void Set(class Application* app, class ResourceMgr* mgr, const char* filename) = 0;
		virtual void Operate() = 0;
	};

	virtual ~IResourceObject() = default;

	/**
		This should return whether or not the resource is ready for use.
	 **/
	virtual bool IsReady() const = 0;

	virtual ILoadOp* GetLoadOperation() const = 0;
};

using ResourcePtr = eastl::shared_ptr<IResourceObject>;

CLOSE_NAMESPACE(Firestorm);
#endif