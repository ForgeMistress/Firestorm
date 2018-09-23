///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SceneGraphResource
//
//  Loads up the scene graph for a given mesh.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_SCENEGRAPHRESOURCE_H_
#define LIBSCENE_SCENEGRAPHRESOURCE_H_
#pragma once

#include "RenderMgr.h"

#include <libIO/ResourceMgr.h>
#include <libIO/ResourceLoader.h>
#include <libIO/IResourceObject.h>
#include <libIO/ResourceReference.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SceneGraphLoader final : public ResourceLoader
{
public:
	SceneGraphLoader(RenderMgr& renderMgr);
	virtual ~SceneGraphLoader();

	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref) override;

private:
	RenderMgr&                           _renderMgr;
	Json::CharReaderBuilder              _builder;
	Json::CharReader*                    _reader;
	ObjectPool<class SceneGraphResource> _pool;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SceneGraphResource final : public IResourceObject
{
	FIRE_RESOURCE_TYPE(SceneGraphResource, SceneGraphLoader);
public:
	struct AssetData
	{
		String Version;
		String Generator;
		String Copyright;
	};

	struct Buffer
	{
		// MeshResource
		ResourceReference Resource;
	};

	struct BufferView
	{
		size_t Index;
		size_t ByteLength;
		size_t ByteOffset;
	};
public:
	SceneGraphResource(RenderMgr& renderMgr);
	virtual ~SceneGraphResource();

private:
	RenderMgr&                _renderMgr;
	AssetData                 _assetData;
	Vector<Buffer>            _buffers;
	Vector<BufferView>        _bufferViews;
};

CLOSE_NAMESPACE(Firestorm);

#endif