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

#include "../RenderMgr.h"
#include "../Enumerations.h"

#include <libApp/Application.h>
#include <libIO/libIO.h>
#include <libIO/ResourceIOErrors.h>
#include <libIO/ResourceMgr.h>

#include <json/json.h>
#include <json/reader.h>

OPEN_NAMESPACE(Firestorm);

FIRE_RESOURCE_DEFINE(Vk_Shader)
{
	auto& renderSystem = App->GetSystems().GetRenderMgr().GetSystem();

	const string filename = ResourceRef.GetPath();
	if(libIO::FileExists(filename.c_str()))
	{
		const string extension = ResourceRef.GetExtension();
		Result<vector<char>, Error> loadedFileResult(libIO::LoadFile(filename));
		if(loadedFileResult.has_value())
		{
			ShaderType type;
			if(extension == "vert.spv") type = ShaderType::kVERTEX;
			if(extension == "frag.spv") type = ShaderType::kFRAGMENT;
			if(extension == "geom.spv") type = ShaderType::kGEOMETRY;
			if(extension == "comp.spv") type = ShaderType::kCOMPUTE;

			renderSystem.ResourceInitialize(ResourcePtr.get(), IShader::CreateInfo{
				type,
				loadedFileResult.value()
			});

			FIRE_LOG_DEBUG("!!!!! LOAD LOGIC FOR Vk_Shader RUN! '%s'", filename.c_str());
		}
		return FIRE_LOAD_FAIL(ResourceIOErrors::FILE_READ_ERROR, Format("could not read file '%s'", filename.c_str()));
	}
	return FIRE_LOAD_FAIL(ResourceIOErrors::FILE_NOT_FOUND_ERROR, Format("could not find file '%s'", filename.c_str()));
}

Vk_Shader::Vk_Shader(Application& app)
: _renderSystem(app.GetSystems().GetRenderMgr().GetSystem())
, _shader(VK_NULL_HANDLE)
, _type(ShaderType::kVERTEX)
{
}

Vk_Shader::~Vk_Shader()
{
}

bool Vk_Shader::IsReady() const
{
	return _shader != VK_NULL_HANDLE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FIRE_RESOURCE_DEFINE(Vk_ShaderProgram)
{
	auto& renderSystem = App->GetSystems().GetRenderMgr().GetSystem();
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

			vector<Resource<IShader>> shaders;
			if(root.isMember("vertex"))
			{
				string value(root["vertex"].asCString());
				if(!libIO::FileExists(value.c_str()))
				{
					return FIRE_LOAD_FAIL(
						ResourceIOErrors::FILE_READ_ERROR, 
						"vertex shader file could not be found");
				}
				//ResourcePtr->AddShader(Mgr->Load<IShader>(value.c_str()));
				shaders.push_back(Mgr->Load<IShader>(value.c_str()));
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
				//ResourcePtr->AddShader(Mgr->Load<IShader>(value.c_str()));
				shaders.push_back(Mgr->Load<IShader>(value.c_str()));
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
				//ResourcePtr->AddShader(Mgr->Load<IShader>(value.c_str()));
				shaders.push_back(Mgr->Load<IShader>(value.c_str()));
			}

			FIRE_LOG_DEBUG("!!!!! LOAD LOGIC FOR Vk_ShaderProgram RUN! '%s'", filename.c_str());
			renderSystem.ResourceInitialize(ResourcePtr.get(), IShaderProgram::CreateInfo{shaders});
			return FIRE_LOAD_SUCCESS;
		}
	}
	return FIRE_LOAD_FAIL(ResourceIOErrors::FILE_NOT_FOUND_ERROR, "not found, dummy");
}

Vk_ShaderProgram::Vk_ShaderProgram(Application& app)
: _renderMgr(app.GetSystems().GetRenderMgr())
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vk_ShaderProgram::~Vk_ShaderProgram()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Vk_ShaderProgram::IsReady() const
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


CLOSE_NAMESPACE(Firestorm);