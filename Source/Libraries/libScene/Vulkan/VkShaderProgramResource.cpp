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
#include "VkShaderProgramResource.h"

#include <libIO/libIO.h>
#include <libIO/ResourceIOErrors.h>
#include <libIO/ResourceMgr.h>

OPEN_NAMESPACE(Firestorm);

FIRE_RESOURCE_DEFINE(Shader)
{
	const string filename = ResourceRef.GetPath();
	FIRE_LOG_DEBUG("!! loading shader '%s'", filename.c_str());
	if(libIO::FileExists(filename.c_str()))
	{
		FIRE_LOG_DEBUG("!!!!! LOAD LOGIC FOR Shader RUN! '%s'", filename.c_str());
	}
	return FIRE_LOAD_FAIL(ResourceIOErrors::FILE_NOT_FOUND_ERROR, Format("could not find file '%s'", filename.c_str()));
}

Shader::Shader(Application& app)
: _renderMgr(app.GetSystems().GetRenderMgr())
{

}

Shader::~Shader()
{
}

bool Shader::IsReady() const
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FIRE_RESOURCE_DEFINE(ShaderProgram)
{
	Json::CharReaderBuilder charReaderBuilder;
	RefPtr<Json::CharReader> _reader(charReaderBuilder.newCharReader());

	const string filename = ResourceRef.GetPath();
	if(libIO::FileExists(filename.c_str()))
	{
		Result<vector<char>, Error> result = libIO::LoadFile(filename);
		if(result.has_value())
		{
			const vector<char>& data = result.value();
			Json::Value root;
			JSONCPP_STRING e;
			if(!_reader->parse(&data[0], &data[data.size() - 1], &root, &e))
			{
				FIRE_LOG_ERROR("!! failed! parsing json");
				string errors(e.c_str());
				return FIRE_LOAD_FAIL(ResourceIOErrors::PARSING_ERROR, errors);
			}

			if(root.isMember("vertex"))
			{
				string value(root["vertex"].asCString());
				FIRE_LOG_DEBUG("    :: Loading Vertex Shader %s", value);
				if(!libIO::FileExists(value.c_str()))
				{
					FIRE_LOG_ERROR("!! failed! loading vertex...");
					return FIRE_LOAD_FAIL(
						ResourceIOErrors::FILE_READ_ERROR, 
						"vertex shader could not be loaded");
				}
				Resource->AddShader(ResMgr.LoadDependency<Shader>(value.c_str()));
			}
			
			if(root.isMember("fragment"))
			{
				string value(root["fragment"].asCString());
				FIRE_LOG_DEBUG("    :: Loading Fragment Shader %s", value);
				if(!libIO::FileExists(value.c_str()))
				{
					FIRE_LOG_ERROR("!! failed! loading fragment...");
					return FIRE_LOAD_FAIL(
						ResourceIOErrors::FILE_READ_ERROR, 
						"fragment shader could not be loaded");
				}
				Resource->AddShader(ResMgr.LoadDependency<Shader>(value.c_str()));
			}
			
			if(root.isMember("geometry"))
			{
				string value(root["geometry"].asCString());
				FIRE_LOG_DEBUG("    :: Loading Geometry Shader %s", value);
				Result<string, Error> result = libIO::LoadFileString(value);
				if(!libIO::FileExists(value.c_str()))
				{
					return FIRE_LOAD_FAIL(
						ResourceIOErrors::FILE_READ_ERROR, 
						"geometry shader could not be loaded");
				}
				Resource->AddShader(ResMgr.LoadDependency<Shader>(value.c_str()));
			}
			FIRE_LOG_DEBUG("!!!!! LOAD LOGIC FOR ShaderProgram RUN! '%s'", filename.c_str());
		}
	}
	return FIRE_LOAD_FAIL(ResourceIOErrors::FILE_NOT_FOUND_ERROR, "not found, dummy");
}

ShaderProgram::ShaderProgram(Application& app)
: _renderMgr(app.GetSystems().GetRenderMgr())
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderProgram::~ShaderProgram()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderProgram::IsReady() const
{
	for(const auto& shader : _shaders)
	{
		if(shader.IsFinished() && shader->IsReady())
		{
			continue;
		}
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderProgram::AddShader(Resource<Shader>& shader)
{
	_shaders.push_back(shader);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);