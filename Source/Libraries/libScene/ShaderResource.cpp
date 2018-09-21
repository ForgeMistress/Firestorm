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

FIRE_MIRROR_DEFINE(ShaderResource){}

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

ResourceMgr::LoadResult ShaderLoader::Load(const ResourceReference& ref)
{
	const String& filename = ref.GetResourcePath();
	if(!libIO::FileExists(filename))
	{
		Result<Vector<char>, Error> result = libIO::LoadFile(filename);
		if (result.has_value())
		{
			const Vector<char>& data = result.value();
			Json::Value root;
			JSONCPP_STRING errors;
			if(!_reader->parse(&data[0], &data[data.size()], &root, &errors))
			{
				return FIRE_ERROR(ResourceIOErrors::kParsingException, errors);
			}

			ShaderResource* shaderResource = new ShaderResource(_renderMgr);
			std::unordered_map<LLGL::ShaderType, LLGL::Shader*> shaders;

			if(_renderMgr.IsUsingRenderer(Renderers::OpenGL))
			{
				if(root.isMember(Renderers::OpenGL))
				{
					auto openGL = root[Renderers::OpenGL];

					if(openGL.isMember("vertex"))
					{
						auto value = openGL["vertex"].asString();
						FIRE_LOG_DEBUG("    :: Loading Vertex Shader %s", value);
						Result<Vector<char>, Error> result = libIO::LoadFile(value);
						if(!result.has_value())
						{
							delete shaderResource;
							return FIRE_ERROR(ResourceIOErrors::kFileRead, "fragment shader could not be loaded");
						}

						LLGL::Shader* shader = MakeShader(result.value(), LLGL::ShaderType::Vertex);
						if(shader->HasErrors())
						{
							_renderMgr.System->Release(*shader);
							delete shaderResource;
							return FIRE_ERROR(ResourceIOErrors::kParsingException, shader->QueryInfoLog());
						}
						shaders[LLGL::ShaderType::Vertex] = shader;
					}

					if(openGL.isMember("fragment"))
					{
						auto value = openGL["fragment"].asString();
						FIRE_LOG_DEBUG("    :: Loading Fragment Shader %s", value);
						Result<Vector<char>, Error> result = libIO::LoadFile(value);
						if(!result.has_value())
						{
							delete shaderResource;
							return FIRE_ERROR(ResourceIOErrors::kFileRead, "fragment shader could not be loaded");
						}

						LLGL::Shader* shader = MakeShader(result.value(), LLGL::ShaderType::Vertex);
						if(shader->HasErrors())
						{
							_renderMgr.System->Release(*shader);
							delete shaderResource;
							return FIRE_ERROR(ResourceIOErrors::kParsingException, shader->QueryInfoLog());
						}
						shaders[LLGL::ShaderType::Vertex] = shader;
					}

					if(openGL.isMember("geometry"))
					{
						auto value = openGL["geometry"].asString();
						FIRE_LOG_DEBUG("    :: Loading Geometry Shader %s", value);
						Result<Vector<char>, Error> result = libIO::LoadFile(value);
						if(!result.has_value())
						{
							delete shaderResource;
							return FIRE_ERROR(ResourceIOErrors::kFileRead, "geometry shader could not be loaded");
						}

						LLGL::Shader* shader = MakeShader(result.value(), LLGL::ShaderType::Geometry);
						if(shader->HasErrors())
						{
							_renderMgr.System->Release(*shader);
							delete shaderResource;
							return FIRE_ERROR(ResourceIOErrors::kParsingException, shader->QueryInfoLog());
						}
						shaders[LLGL::ShaderType::Geometry] = shader;
					}
				}
			}
			else if(_renderMgr.IsUsingRenderer(Renderers::Direct3D))
			{
				FIRE_ASSERT(false && "direct3D support is not yet implemented");
			}

			// plug them all in to the shaderresource and build the shaderprogram.

			LLGL::ShaderProgramDescriptor desc;
			desc.vertexShader   = shaders[LLGL::ShaderType::Vertex];
			desc.fragmentShader = shaders[LLGL::ShaderType::Fragment];
			desc.geometryShader = shaders[LLGL::ShaderType::Geometry];

			LLGL::ShaderProgram* shaderProgram = _renderMgr.System->CreateShaderProgram(desc);
			if(shaderProgram->HasErrors())
			{
				delete shaderResource;
				_renderMgr.System->Release(*shaderProgram);
				return FIRE_ERROR(ResourceIOErrors::kProcessingException, shaderProgram->QueryInfoLog());
			}

			for(auto i : shaders)
			{
				shaderResource->_shaders.push_back(i.second);
			}
			shaderResource->_shaderProgram = shaderProgram;
			
			return shaderResource;
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

LLGL::Shader* ShaderLoader::MakeShader(const Vector<char>& data, LLGL::ShaderType shaderType)
{
	LLGL::ShaderDescriptor desc;
	desc.source = &data[0];
	desc.sourceSize = data.size();
	desc.type = shaderType;
	return _renderMgr.System->CreateShader(desc);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);