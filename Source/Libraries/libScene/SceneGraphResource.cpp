///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SceneGraphResource
//
//  Loads up the scene graph for a given mesh.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SceneGraphResource.h"
#include "MeshResource.h"

#include <libIO/ResourceIOErrors.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SceneGraphResource::SceneGraphResource(RenderMgr& renderMgr)
: _renderMgr(renderMgr)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SceneGraphResource::~SceneGraphResource()
{
	_buffers.clear();
	_bufferViews.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SceneGraphLoader::SceneGraphLoader(RenderMgr& renderMgr)
: _renderMgr(renderMgr)
{
	_builder["collectComments"] = false;
	_reader = _builder.newCharReader();
}

SceneGraphLoader::~SceneGraphLoader()
{
	delete _reader;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceLoader::LoadResult SceneGraphLoader::Load(ResourceMgr* resourceMgr, const ResourceReference& ref)
{
	auto path = ref.GetResourcePath();
	if(libIO::FileExists(path))
	{
		auto result = libIO::LoadFile(path);
		if(result.has_value())
		{
			auto data = result.value();
			FIRE_ASSERT(!data.empty());

			JSONCPP_STRING errors;
			Json::Value root;
			if(!_reader->parse(&data[0], &data[data.size() - 1], &root, &errors))
			{
				return FIRE_ERROR(ResourceIOErrors::PARSING_ERROR, errors);
			}

			FIRE_ASSERT_MSG(root.isMember("asset"), "not a valid gltf file. no 'asset' block.");

			// Read the asset block.
			auto asset = root["asset"];
			SceneGraphResource* resource = _pool.Get(_renderMgr);
			resource->_assetData.Version = asset.get("version", "0.0.0").asString();
			resource->_assetData.Copyright = asset.get("copyright", "").asString();
			resource->_assetData.Generator = asset.get("generator", "<unknown>").asString();

			// now read the buffers and buffer views.
			if(root.isMember("buffers"))
			{
				auto buffers = root["buffers"];
				for(size_t i = 0; i < buffers.size(); ++i)
				{
					// resolve the location of the uri.
					ResourceReference meshRef(ref.GetPathTo() + buffers[i]["uri"].asString());
					// kick off a deferred load of the mesh.
					//resourceMgr->Load<MeshResource>(meshRef);
					resource->_buffers.push_back(SceneGraphResource::Buffer{
						std::move(meshRef)
					});
				}
			}

			if(root.isMember("bufferViews"))
			{
				auto bufferViews = root["bufferViews"];
				for(size_t i = 0; i < bufferViews.size(); ++i)
				{
					auto bufferView = bufferViews[i];
					resource->_bufferViews.push_back(SceneGraphResource::BufferView{
						bufferView["buffer"].asUInt(),
						bufferView["byteLength"].asUInt(),
						bufferView["byteOffset"].asUInt()
					});
				}
			}
			auto dest = [this](IResourceObject* ptr) {
				_pool.Return(ptr);
			};
			return FIRE_RESULT(std::make_pair(resource, dest));
		}
		return FIRE_ERROR(ResourceIOErrors::PARSING_ERROR, (String)result.error());
	}
	return FIRE_ERROR(ResourceIOErrors::FILE_NOT_FOUND_ERROR, path);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);