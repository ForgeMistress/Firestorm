///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SceneGraphResource
//
//  Loads up the scene graph for a given mesh.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_SCENEGRAPHRESOURCE_H_
#define LIBSCENE_SCENEGRAPHRESOURCE_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

class SceneGraphResource : public IResourceObject
{
	FIRE_MIRROR_DECLARE(SceneGraphResource, IResourceObject);
public:
	SceneGraphResource();
	virtual ~SceneGraphResource();


	virtual Result<void, Error> Load(const Json::Value& inValue);
	virtual Result<void, Error> Save(Json::Value& outValue) const;
private:
};

CLOSE_NAMESPACE(Firestorm);

#endif