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

ShaderResource::ShaderResource(LLGL::RenderSystem* renderSystem)
: _renderSystem(renderSystem)
{
	FIRE_ASSERT(_renderSystem != nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderResource::~ShaderResource()
{
	for(auto shader : _shaders)
	{
		_renderSystem->Release(*shader);
	}
	_renderSystem->Release(*_shaderProgram);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderLoader::ShaderLoader(ResourceReference* res, LLGL::RenderSystem* system)
: _resHandle(res)
, _system(system)
{
	_builder["collectComments"] = false;
	_reader = _builder.newCharReader();
}

ShaderLoader::~ShaderLoader()
{
	delete _reader;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Result<RefPtr<IResourceObject>, Error> ShaderLoader::operator()()
{
	const String& filename = _resHandle->GetResourcePath();
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);