///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderProgramResource
//
//  Loads up and stores Shader data.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_SHADERPROGRAMRESOURCE_H_
#define LIBSCENE_SHADERPROGRAMRESOURCE_H_
#pragma once

#include "RenderMgr.h"

#include <libIO/ResourceMgr.h>
#include <libIO/ResourceLoader.h>
#include <libIO/IResourceObject.h>
#include <libIO/ResourceReference.h>
#include <libIO/ResourceHandle.h>

#include <json/json.h>
#include <json/reader.h>
#include <libCore/ObjectPool.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderProgramLoader final : public ResourceLoader
{
public:
	ShaderProgramLoader(RenderMgr& renderMgr);
	~ShaderProgramLoader();

	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref) override;
private:
	RenderMgr&                              _renderMgr;
	Json::CharReaderBuilder                 _builder;
	Json::CharReader*                       _reader;
	ObjectPool<class ShaderProgramResource> _shaderPool;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderProgramResource final : public IResourceObject
{
	FIRE_RESOURCE_TYPE(ShaderProgramResource, ShaderProgramLoader);
public:
	ShaderProgramResource(RenderMgr& renderMgr);
	virtual ~ShaderProgramResource();

	virtual bool IsReady() const;
private:
	RenderMgr&                                    _renderMgr;
	bool                                          _isCompiled{ false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif