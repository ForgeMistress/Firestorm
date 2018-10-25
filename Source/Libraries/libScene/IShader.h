///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IShader
//
//  IShader 
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_ISHADER_H_
#define LIBSCENE_ISHADER_H_
#pragma once

#include "Enumerations.h"
#include <libIO/ResourceHandle.h>
#include <libIO/IResourceObject.h>

OPEN_NAMESPACE(Firestorm);

class IShader : public IResourceObject
{
public:
	struct CreateInfo
	{
		ShaderType   Type{ ShaderType::kVERTEX };
		vector<char> Data;
	};

	virtual ~IShader() = default;

	virtual ShaderType GetType() const = 0;
};

class IShaderProgram : public IResourceObject
{
public:
	struct CreateInfo
	{
		vector<Resource<IShader>> Shaders;
	};

	virtual ~IShaderProgram() = default;
};

CLOSE_NAMESPACE(Firestorm);

#endif