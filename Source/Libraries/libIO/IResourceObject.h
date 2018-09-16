////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//<
//<  \file ResourceObject.h
//<
//<  Contains \ref IResourceObject
//<
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEOBJECT_H_
#define LIBIO_RESOURCEOBJECT_H_
#pragma once

#include <json/value.h>

OPEN_NAMESPACE(Firestorm);

class IDocument;

class IResourceObject
{
	FIRE_MIRROR_DECLARE(IResourceObject);
public:
	virtual ~IResourceObject() {}

	/**
		Decode the data from the buffer that is provided.
	 **/
	virtual Result<void, Error> Decode(const Vector<char>& inDataBuffer) = 0;

	/**
		Encode the data into the provided data buffer.
	 **/
	virtual Result<void, Error> Encode(Vector<char>& outDataBuffer) = 0;
};

CLOSE_NAMESPACE(Firestorm);
#endif