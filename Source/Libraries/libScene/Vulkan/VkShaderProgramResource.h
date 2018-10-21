///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderProgramResource
//
//  Loads up and stores Shader data.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_VKSHADERPROGRAMRESOURCE_H_
#define LIBSCENE_VKSHADERPROGRAMRESOURCE_H_
#pragma once

#include "../RenderMgr.h"

#include <libApp/Application.h>
#include <libIO/ResourceMgr.h>
#include <libIO/ResourceLoader.h>
#include <libIO/IResourceObject.h>
#include <libIO/ResourceReference.h>
#include <libIO/ResourceHandle.h>

#include <json/json.h>
#include <json/reader.h>
#include <libCore/ObjectPool.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Shader final : public IResourceObject
{
	FIRE_RESOURCE_DECLARE(Shader);
public:
	Shader(Application& app);
	virtual ~Shader();

	virtual bool IsReady() const;
private:
	RenderMgr& _renderMgr;
	bool       _isCompiled{ false };
};

class ShaderProgram final : public IResourceObject
{
	FIRE_RESOURCE_DECLARE(ShaderProgram);
public:
	ShaderProgram(Application& app);
	virtual ~ShaderProgram();

	virtual bool IsReady() const;
	void PrintNumShaders() const { FIRE_LOG_DEBUG("!! Shader program has %d shaders.", _shaders.size()); }


private:
	void AddShader(Resource<Shader>& shader);

	RenderMgr&                _renderMgr;
	std::mutex                _slock;
	vector<Resource<Shader>> _shaders;
	bool                     _isCompiled{ false };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);

#endif