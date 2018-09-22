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
		_renderMgr.System->Release(*shader.second);
	}
	_renderMgr.System->Release(*_shaderProgram);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LLGL::ShaderProgram* ShaderResource::GetProgram() const
{
	FIRE_ASSERT(_isCompiled);
	return _shaderProgram;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderResource::Compile()
{
	FIRE_LOG_DEBUG("Compiling Shader...");
	// create the shaders...
	for(auto shaderData : _shaderData)
	{
		auto type = shaderData.first;
		auto data = shaderData.second;
		data.push_back('\0');
		LLGL::ShaderDescriptor desc;
		desc.type = type;
		desc.source = &data[0];
		desc.sourceSize = data.size();
		desc.sourceType = LLGL::ShaderSourceType::CodeString;
		_shaders[type] = _renderMgr.System->CreateShader(desc);
		if (_shaders[type]->HasErrors())
		{
			FIRE_LOG_ERROR("Error Creating Shader: %s", _shaders[type]->QueryInfoLog());
			PurgeCompiledShaders();
			return false;
		}
	}

	LLGL::ShaderProgramDescriptor desc;
	if(_shaders.find(LLGL::ShaderType::Vertex) != _shaders.end())
	{
		desc.vertexShader = _shaders[LLGL::ShaderType::Vertex];
	}
	if(_shaders.find(LLGL::ShaderType::Fragment) != _shaders.end())
	{
		desc.fragmentShader = _shaders[LLGL::ShaderType::Fragment];
	}
	if(_shaders.find(LLGL::ShaderType::Geometry) != _shaders.end())
	{
		desc.geometryShader = _shaders[LLGL::ShaderType::Geometry];
	}

	_shaderProgram = _renderMgr.System->CreateShaderProgram(desc);
	if(_shaderProgram->HasErrors())
	{
		FIRE_LOG_ERROR("Error Compiling Shader Program: %s", _shaderProgram->QueryInfoLog());
		_renderMgr.System->Release(*_shaderProgram);
		PurgeCompiledShaders();
		return false;
	}
	_isCompiled = true;
	return true;
}

void ShaderResource::PurgeCompiledShaders()
{
	for(auto shader : _shaders)
	{
		if(shader.second)
			_renderMgr.System->Release(*shader.second);
	}
	if(_shaderProgram)
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
	if(libIO::FileExists(filename))
	{
		Result<Vector<char>, Error> result = libIO::LoadFile(filename);
		if (result.has_value())
		{
			const Vector<char>& data = result.value();
			Json::Value root;
			JSONCPP_STRING errors;
			if(!_reader->parse(&data[0], &data[data.size() - 1], &root, &errors))
			{
				return FIRE_ERROR(ResourceIOErrors::PARSING_ERROR, errors);
			}

			ShaderResource* shaderResource = _shaderPool.Get(_renderMgr);
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
							return FIRE_ERROR(ResourceIOErrors::FILE_READ_ERROR, "fragment shader could not be loaded");
						}
						shaderResource->_shaderData[LLGL::ShaderType::Vertex] = result.value();
					}

					if(openGL.isMember("fragment"))
					{
						auto value = openGL["fragment"].asString();
						FIRE_LOG_DEBUG("    :: Loading Fragment Shader %s", value);
						Result<Vector<char>, Error> result = libIO::LoadFile(value);
						if(!result.has_value())
						{
							return FIRE_ERROR(ResourceIOErrors::FILE_READ_ERROR, "fragment shader could not be loaded");
						}

						shaderResource->_shaderData[LLGL::ShaderType::Fragment] = result.value();
					}

					if(openGL.isMember("geometry"))
					{
						auto value = openGL["geometry"].asString();
						FIRE_LOG_DEBUG("    :: Loading Geometry Shader %s", value);
						Result<Vector<char>, Error> result = libIO::LoadFile(value);
						if(!result.has_value())
						{
							return FIRE_ERROR(ResourceIOErrors::FILE_READ_ERROR, "geometry shader could not be loaded");
						}
						shaderResource->_shaderData[LLGL::ShaderType::Fragment] = result.value();
					}
				}
			}
			else if(_renderMgr.IsUsingRenderer(Renderers::Direct3D))
			{
				FIRE_ASSERT(false && "direct3D support is not yet implemented");
			}

			// return the shader resource now, because the shader has to be actually vompiled on the main thread.
			return RefPtr<IResourceObject>(shaderResource, [this](IResourceObject* ptr) {
				_shaderPool.Return(reinterpret_cast<ShaderResource*>(ptr));
			});
		}
		else
		{
			return FIRE_ERROR(ResourceIOErrors::FILE_READ_ERROR, ((String)result.error()));
		}
	}
	else
	{
		return FIRE_ERROR(ResourceIOErrors::FILE_NOT_FOUND_ERROR, filename);
	}
	return FIRE_ERROR(ResourceIOErrors::PROCESSING_ERROR);
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