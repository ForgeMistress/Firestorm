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

#include <libApp/Application.h>
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

//class ShaderProgramLoader final : public ResourceLoader
//{
//public:
//	ShaderProgramLoader(RenderMgr& renderMgr);
//	~ShaderProgramLoader();
//
//	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref) override;
//private:
//	RenderMgr&                              _renderMgr;
//	Json::CharReaderBuilder                 _builder;
//	Json::CharReader*                       _reader;
//	ObjectPool<class ShaderProgramResource> _shaderPool;
//};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct IResourceLoadOp
{
	virtual LoadResult operator()(tf::SubflowBuilder& dependencies) = 0;
};


class ShaderProgram final : public IResourceObject
{
	friend class ResourceMgr;
	FIRE_MIRROR_DECLARE(ShaderProgram);
	struct LoadOp : public IResourceLoadOp
	{
		ResourceReference ResourceRef;
		Application& App;
		eastl::shared_ptr<ShaderProgram> Resource;

		LoadOp(Application& app, const char* filename, eastl::shared_ptr<ShaderProgram> resource)
		: ResourceRef(filename)
		, App(app)
		, Resource(resource)
		{
		}
		LoadResult operator()(tf::SubflowBuilder& dependencies);
	};
	//static IResourceLoadOp* LoadOp(Application& app, const char* filename) { return new ShaderLoadOp(app, filename); }
public:
	ShaderProgram(Application& app);
	virtual ~ShaderProgram();

	virtual bool IsReady() const;
private:
	RenderMgr&                                    _renderMgr;
	bool                                          _isCompiled{ false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif