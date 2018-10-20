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

//ShaderProgramLoader::ShaderProgramLoader(RenderMgr& renderMgr)
//	: _renderMgr(renderMgr)
//{
//	_builder["collectComments"] = false;
//	_reader = _builder.newCharReader();
//}
//
//ShaderProgramLoader::~ShaderProgramLoader()
//{
//	delete _reader;
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//ShaderProgramLoader::LoadResult ShaderProgramLoader::Load(ResourceMgr* resourceMgr, const ResourceReference& ref)
//{
//	const string& filename = ref.GetResourcePath();
//	/*if(libIO::FileExists(filename.c_str()))
//	{
//		Result<vector<char>, Error> result = libIO::LoadFile(filename);
//		if(result.has_value())
//		{
//			const vector<char>& data = result.value();
//			Json::Value root;
//			JSONCPP_STRING e;
//			if(!_reader->parse(&data[0], &data[data.size() - 1], &root, &e))
//			{
//				string errors(e.c_str());
//				return FIRE_LOAD_FAIL(
//					ResourceIOErrors::PARSING_ERROR,
//					errors);
//			}
//
//			RefPtr<ShaderProgramResource> shaderResource(make_shared<ShaderProgramResource>(_renderMgr));
//
//			if(_renderMgr.IsUsingRenderer(Renderers::OpenGL))
//			{
//				if(root.isMember(Renderers::OpenGL))
//				{
//					auto openGL = root[Renderers::OpenGL];
//
//					if(openGL.isMember("vertex"))
//					{
//						string value(openGL["vertex"].asCString());
//						FIRE_LOG_DEBUG("    :: Loading Vertex Shader %s", value);
//						Result<string, Error> result = libIO::LoadFileString(value);
//						if(!result.has_value())
//						{
//							return FIRE_LOAD_FAIL(
//								ResourceIOErrors::FILE_READ_ERROR, 
//								"fragment shader could not be loaded");
//						}
//						shaderResource->AddShaderData(LLGL::ShaderType::Vertex, result.value());
//					}
//
//					if(openGL.isMember("fragment"))
//					{
//						string value(openGL["fragment"].asCString());
//						FIRE_LOG_DEBUG("    :: Loading Fragment Shader %s", value);
//						Result<string, Error> result = libIO::LoadFileString(value);
//						if(!result.has_value())
//						{
//							return FIRE_LOAD_FAIL(
//								ResourceIOErrors::FILE_READ_ERROR,
//								"fragment shader could not be loaded");
//						}
//
//						shaderResource->AddShaderData(LLGL::ShaderType::Fragment, result.value());
//					}
//
//					if(openGL.isMember("geometry"))
//					{
//						string value(openGL["geometry"].asCString());
//						FIRE_LOG_DEBUG("    :: Loading Geometry Shader %s", value);
//						Result<string, Error> result = libIO::LoadFileString(value);
//						if(!result.has_value())
//						{
//							return FIRE_LOAD_FAIL(
//								ResourceIOErrors::FILE_READ_ERROR,
//								"geometry shader could not be loaded");
//						}
//						shaderResource->AddShaderData(LLGL::ShaderType::Geometry, result.value());
//					}
//				}
//			}
//			else if(_renderMgr.IsUsingRenderer(Renderers::Direct3D))
//			{
//				FIRE_ASSERT_MSG(false, "direct3D support is not yet implemented");
//			}
//
//			// return the shader resource now, because the shader has to be actually compiled on the main thread.
//			return FIRE_LOAD_SUCCESS(shaderResource);
//		}
//		return FIRE_LOAD_FAIL(
//			ResourceIOErrors::FILE_READ_ERROR,
//			((string)result.error()));
//	}*/
//	return FIRE_LOAD_FAIL(
//		ResourceIOErrors::FILE_NOT_FOUND_ERROR,
//		filename);
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LoadResult ShaderProgram::LoadOp::operator()(tf::SubflowBuilder& dependencies)
{
	static Json::CharReaderBuilder  _s_charReaderBuilder;

	RefPtr<Json::CharReader> _reader(_s_charReaderBuilder.newCharReader());

	const string& filename = ResourceRef.GetResourcePath();
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

			//RefPtr<ShaderProgram> shaderProgram(make_shared<ShaderProgram>(Mgr));
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
				FIRE_LOG_DEBUG("@@ Vertex file loaded!");
				//Result<string, Error> result = libIO::LoadFileString(value);
				//if(!result.has_value())
				//{
				//	return FIRE_LOAD_FAIL(
				//		ResourceIOErrors::FILE_READ_ERROR, 
				//		"fragment shader could not be loaded");
				//}
				//shaderResource->AddShaderData(LLGL::ShaderType::Vertex, result.value());
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
				//Result<string, Error> result = libIO::LoadFileString(value);
				//if(!result.has_value())
				//{
				//	return FIRE_LOAD_FAIL(
				//		ResourceIOErrors::FILE_READ_ERROR,
				//		"fragment shader could not be loaded");
				//}
			
				//shaderProgram->AddShaderData(LLGL::ShaderType::Fragment, result.value());
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
				//if(!result.has_value())
				//{
				//	return FIRE_LOAD_FAIL(
				//		ResourceIOErrors::FILE_READ_ERROR,
				//		"geometry shader could not be loaded");
				//}
				//shaderProgram->AddShaderData(LLGL::ShaderType::Geometry, result.value());
			}
			return Resource;
		}
	}
	return FIRE_LOAD_FAIL(ResourceIOErrors::FILE_NOT_FOUND_ERROR, "not found, dummy");
}

ShaderProgram::ShaderProgram(Application& app)
: _renderMgr(app.GetSystems().RenderMgr())
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ShaderProgram::~ShaderProgram()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderProgram::IsReady() const
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);