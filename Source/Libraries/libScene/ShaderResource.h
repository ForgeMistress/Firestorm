///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderResource
//
//  Loads up and stores Shader data.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_SHADERRESOURCE_H_
#define LIBSCENE_SHADERRESOURCE_H_
#pragma once

#include <libIO/ResourceReference.h>
#include <libIO/FileIOMgr.h>
#include <libIO/ResourceReference.h>

#include <json/json.h>
#include <json/reader.h>
#include <libCore/ObjectPool.h>

#include "RenderMgr.h"

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderResource final
{
	FIRE_MIRROR_DECLARE(ShaderResource);
	friend struct ShaderLoader;
public:
	ShaderResource(RenderMgr& renderMgr);
	virtual ~ShaderResource();

private:
	RenderMgr& _renderMgr;
	Vector<LLGL::Shader*> _shaders;
	LLGL::ShaderProgram* _shaderProgram{ nullptr };
};

struct ShaderLoader;

struct ShaderLoaderMaker : public IMaker
{
	ShaderLoaderMaker(RenderMgr& renderMgr)
	: _renderMgr(renderMgr) 
	{
	}

	virtual void* Make() override
	{
		return _pool.Get(_renderMgr);
	}

	RenderMgr& _renderMgr;
	ObjectPool<ShaderLoader> _pool;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ShaderLoader final
{
	ShaderLoader(RenderMgr& renderMgr);
	~ShaderLoader();

	ResourceLoadResult_t operator()(const ResourceReference& ref);
private:
	RenderMgr&              _renderMgr;
	Json::CharReaderBuilder _builder;
	Json::CharReader*       _reader;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <>
struct ResourceTraits<ShaderResource>
{
	using type   = ShaderResource;
	using loader = ShaderLoader;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif