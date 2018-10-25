///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderProgramResource
//
//  Loads up and stores Shader data.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VkShaderProgram.h"

#include "../PrimitiveTopology.h"

#include <libIO/libIO.h>
#include <libIO/ResourceIOErrors.h>
#include <libIO/ResourceMgr.h>

OPEN_NAMESPACE(Firestorm);

FIRE_RESOURCE_DEFINE(Shader)
{
	auto& renderSystem = App.GetSystems().GetRenderMgr().GetSystem();

	const string filename = ResourceRef.GetPath();
	if(libIO::FileExists(filename.c_str()))
	{
		Result<vector<char>, Error> loadedFileResult(libIO::LoadFile(filename));
		if(loadedFileResult.has_value())
		{
			renderSystem.MakeWhole(Resource.get(), loadedFileResult.value());

			FIRE_LOG_DEBUG("!!!!! LOAD LOGIC FOR Shader RUN! '%s'", filename.c_str());
		}
		return FIRE_LOAD_FAIL(ResourceIOErrors::FILE_READ_ERROR, Format("could not read file '%s'", filename.c_str()));
	}
	return FIRE_LOAD_FAIL(ResourceIOErrors::FILE_NOT_FOUND_ERROR, Format("could not find file '%s'", filename.c_str()));
}

Shader::Shader(Application& app, ShaderType type)
: _renderSystem(app.GetSystems().GetRenderMgr().GetSystem())
, _shader(VK_NULL_HANDLE)
, _type(type)
{
}

Shader::~Shader()
{
	_renderSystem.Release(this);
}

bool Shader::IsReady() const
{
	return _shader != VK_NULL_HANDLE;
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
				string errors(e.c_str());
				return FIRE_LOAD_FAIL(ResourceIOErrors::PARSING_ERROR, errors);
			}

			if(root.isMember("vertex"))
			{
				string value(root["vertex"].asCString());
				if(!libIO::FileExists(value.c_str()))
				{
					return FIRE_LOAD_FAIL(
						ResourceIOErrors::FILE_READ_ERROR, 
						"vertex shader file could not be found");
				}
				Resource->AddShader(Mgr.Load<Shader>(value.c_str(), ShaderType::kVERTEX));
			}
			
			if(root.isMember("fragment"))
			{
				string value(root["fragment"].asCString());
				if(!libIO::FileExists(value.c_str()))
				{
					return FIRE_LOAD_FAIL(
						ResourceIOErrors::FILE_READ_ERROR, 
						"fragment shader file could not be found");
				}
				Resource->AddShader(Mgr.Load<Shader>(value.c_str(), ShaderType::kFRAGMENT));
			}
			
			if(root.isMember("geometry"))
			{
				string value(root["geometry"].asCString());
				if(!libIO::FileExists(value.c_str()))
				{
					return FIRE_LOAD_FAIL(
						ResourceIOErrors::FILE_READ_ERROR, 
						"geometry shader file could not be found");
				}
				Resource->AddShader(Mgr.Load<Shader>(value.c_str(), ShaderType::kGEOMETRY));
			}

			FIRE_LOG_DEBUG("!!!!! LOAD LOGIC FOR ShaderProgram RUN! '%s'", filename.c_str());
			Resource->Compile();
			return FIRE_LOAD_SUCCESS;
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
		if(shader.IsFinished())
		{
			continue;
		}
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderProgram::Compile()
{
	vector<VkPipelineShaderStageCreateInfo> stages(_shaders.size());
	for(auto shader : _shaders)
	{
		VkPipelineShaderStageCreateInfo info ={};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		switch(shader->GetType())
		{
		case ShaderType::kVERTEX:
			info.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case ShaderType::kFRAGMENT:
			info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case ShaderType::kGEOMETRY:
			info.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		}
		info.module = shader->_shader;
		info.pName = "main";
		stages.push_back(info);
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderProgram::AddShader(Resource<Shader>& shader)
{
	//if(shader->GetType() == ShaderType::kVertex)
	//{
	//	//VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	//	//vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	//	//vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	//	//vertShaderStageInfo.module = shader->_shader;
	//	//vertShaderStageInfo.pName = "main";
	//}
	_shaders.push_back(shader);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);