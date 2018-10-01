///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderProgramResource
//
//  Loads up and stores Shader data.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ShaderProgramResource.h"

#include <libIO/libIO.h>
#include <libIO/ResourceIOErrors.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderProgramLoader::ShaderProgramLoader(RenderMgr& renderMgr)
	: _renderMgr(renderMgr)
{
	_builder["collectComments"] = false;
	_reader = _builder.newCharReader();
}

ShaderProgramLoader::~ShaderProgramLoader()
{
	delete _reader;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderProgramLoader::LoadResult ShaderProgramLoader::Load(ResourceMgr* resourceMgr, const ResourceReference& ref)
{
	const String& filename = ref.GetResourcePath();
	if(libIO::FileExists(filename))
	{
		Result<Vector<char>, Error> result = libIO::LoadFile(filename);
		if(result.has_value())
		{
			const Vector<char>& data = result.value();
			Json::Value root;
			JSONCPP_STRING errors;
			if(!_reader->parse(&data[0], &data[data.size() - 1], &root, &errors))
			{
				return FIRE_LOAD_FAIL(
					ResourceIOErrors::PARSING_ERROR,
					errors);
			}

			RefPtr<ShaderProgramResource> shaderResource(std::make_shared<ShaderProgramResource>(_renderMgr));

			if(_renderMgr.IsUsingRenderer(Renderers::OpenGL))
			{
				if(root.isMember(Renderers::OpenGL))
				{
					auto openGL = root[Renderers::OpenGL];

					if(openGL.isMember("vertex"))
					{
						auto value = openGL["vertex"].asString();
						FIRE_LOG_DEBUG("    :: Loading Vertex Shader %s", value);
						Result<String, Error> result = libIO::LoadFileString(value);
						if(!result.has_value())
						{
							return FIRE_LOAD_FAIL(
								ResourceIOErrors::FILE_READ_ERROR, 
								"fragment shader could not be loaded");
						}
						shaderResource->AddShaderData(LLGL::ShaderType::Vertex, result.value());
					}

					if(openGL.isMember("fragment"))
					{
						auto value = openGL["fragment"].asString();
						FIRE_LOG_DEBUG("    :: Loading Fragment Shader %s", value);
						Result<String, Error> result = libIO::LoadFileString(value);
						if(!result.has_value())
						{
							return FIRE_LOAD_FAIL(
								ResourceIOErrors::FILE_READ_ERROR,
								"fragment shader could not be loaded");
						}

						shaderResource->AddShaderData(LLGL::ShaderType::Fragment, result.value());
					}

					if(openGL.isMember("geometry"))
					{
						auto value = openGL["geometry"].asString();
						FIRE_LOG_DEBUG("    :: Loading Geometry Shader %s", value);
						Result<String, Error> result = libIO::LoadFileString(value);
						if(!result.has_value())
						{
							return FIRE_LOAD_FAIL(
								ResourceIOErrors::FILE_READ_ERROR,
								"geometry shader could not be loaded");
						}
						shaderResource->AddShaderData(LLGL::ShaderType::Geometry, result.value());
					}
				}
			}
			else if(_renderMgr.IsUsingRenderer(Renderers::Direct3D))
			{
				FIRE_ASSERT_MSG(false, "direct3D support is not yet implemented");
			}

			// return the shader resource now, because the shader has to be actually compiled on the main thread.
			return FIRE_LOAD_SUCCESS(shaderResource);
		}
		return FIRE_LOAD_FAIL(
			ResourceIOErrors::FILE_READ_ERROR,
			((String)result.error()));
	}
	return FIRE_LOAD_FAIL(
		ResourceIOErrors::FILE_NOT_FOUND_ERROR,
		filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderProgramResource::ShaderProgramResource(RenderMgr& renderMgr)
: _renderMgr(renderMgr)
{
	FIRE_ASSERT(_renderMgr.System != nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderProgramResource::~ShaderProgramResource()
{
	PurgeCompiledShaders();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderProgramResource::IsReady() const
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LLGL::ShaderProgram* ShaderProgramResource::GetProgram() const
{
	FIRE_ASSERT(_isCompiled);
	return _shaderProgram;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LLGL::ShaderProgram* ShaderProgramResource::Compile(std::initializer_list<LLGL::VertexFormat> vertexFormats)
{
	if(_isCompiled)
		return _shaderProgram;

	FIRE_LOG_DEBUG("Compiling Shader...");
	// create the shaders...
	for(auto shaderData : _shaderData)
	{
		LLGL::ShaderDescriptor desc;
		desc.source = shaderData.second.c_str();
		desc.sourceSize = 0;
		desc.type = shaderData.first;
		desc.sourceType = LLGL::ShaderSourceType::CodeString;
		desc.flags = LLGL::ShaderCompileFlags::WarnError;
		LLGL::Shader* shader = _renderMgr.System->CreateShader(desc);
		if(!shader)
		{
			PurgeCompiledShaders();
			return nullptr;
		}
		_shaders[shaderData.first] = shader;
	}
	_shaderData.clear();

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

	desc.vertexFormats.reserve(vertexFormats.size());
	for(auto format : vertexFormats)
	{
		desc.vertexFormats.push_back(format);
	}

	_shaderProgram = _renderMgr.System->CreateShaderProgram(desc);
	auto infoLog = _shaderProgram->QueryInfoLog();
	if(!infoLog.empty())
	{
		FIRE_LOG_ERROR("Error Creating Shader Program: %s", infoLog);
		PurgeCompiledShaders();
		return nullptr;
	}

	if(_shaderProgram->HasErrors())
	{
		FIRE_LOG_ERROR("Error Compiling Shader Program: %s", infoLog);
		PurgeCompiledShaders();
		return nullptr;
	}
	_isCompiled = true;
	FIRE_LOG_DEBUG("Shader Compiled Successfully!");
	return _shaderProgram;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderProgramResource::PurgeCompiledShaders()
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

LLGL::Shader* ShaderProgramResource::MakeShader(LLGL::ShaderType shaderType)
{
	auto found = _shaderData.find(shaderType);
	if(found != _shaderData.end())
	{
		LLGL::ShaderDescriptor desc;
		desc.source = found->second.c_str();
		desc.sourceSize = 0;
		desc.type = shaderType;
		desc.sourceType = LLGL::ShaderSourceType::CodeString;
		desc.flags = LLGL::ShaderCompileFlags::WarnError;
		return _renderMgr.System->CreateShader(desc);
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderProgramResource::AddShaderData(LLGL::ShaderType type, const String& data)
{
	if(_shaderData.find(type) != _shaderData.end())
	{
		return false;
	}
	_shaderData[type] = data;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderProgramResource::CompileShader(LLGL::ShaderType type)
{
	if(_shaderData.find(type) != _shaderData.end())
	{
		LLGL::Shader* shader = MakeShader(type);
		if(shader != nullptr)
		{
			_shaderData.erase(type);
			String log = shader->QueryInfoLog();
			if(log.empty())
			{
				if(!shader->HasErrors())
				{
					_shaders[type] = shader;
					return true;
				}
			}
			_renderMgr.System->Release(*shader);
		}
		PurgeCompiledShaders();
		FIRE_LOG_ERROR("Could not compile shader. Shader returned from MakeShader was nullptr...");
		return false;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);