///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderResource
//
//  Loads up and stores Shader data.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ShaderResource.h"

#include <libIO/libIO.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderResource::ShaderResource(RenderMgr& renderMgr)
: _renderMgr(renderMgr)
{
	FIRE_ASSERT(_renderMgr.System != nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderResource::~ShaderResource()
{
	for(auto shader : _shaders)
	{
		_renderMgr.System->Release(*shader);
	}
	_renderMgr.System->Release(*_shaderProgram);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderLoader::ShaderLoader(RenderMgr& renderMgr)
: _renderMgr(renderMgr)
{
	_builder["collectComments"] = false;
	_reader = _builder.newCharReader();
}

ShaderLoader::~ShaderLoader()
{
	delete _reader;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceLoadResult_t ShaderLoader::operator()(const ResourceReference& ref)
{
	const String& filename = ref.GetResourcePath();
	if(!libIO::FileExists(filename))
	{
		Result<Vector<char>, Error> result = libIO::LoadFile(filename);
		if(result.has_value())
		{
			const Vector<char>& data = result.value();
			Json::Value root;
			JSONCPP_STRING errors;
			if(!_reader->parse(&data[0], &data[data.size()], &root, &errors))
			{
				return FIRE_ERROR(ResourceIOErrors::kParsingException, errors);
			}

			if(root.isMember("OpenGL"))
			{
			}
		}
		else
		{
			return FIRE_ERROR(ResourceIOErrors::kFileRead, result.error().Message);
		}
	}
	else
	{
		return FIRE_ERROR(ResourceIOErrors::kFileNotFound, "could not find file " + filename);
	}
	return FIRE_ERROR(ResourceIOErrors::kProcessingException, "");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);