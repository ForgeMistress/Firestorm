////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AABB.h
//
//  An Axis Aligned Bounding Box.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMATH_VECTOR_H_
#define LIBMATH_VECTOR_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

class Vector_2D;
class Vector_3D;

class Vector_2D
{
public:
	Vector_2D(float x=0.0f, float y=0.0f);
	explicit Vector_2D(const Vector_3D& v3d);

	float x;
	float y;
};

class Vector_3D
{
public:
	Vector_3D(float x=0.0f, float y=0.0f, float z=0.0f);
	Vector_3D(const Vector_2D& v2d, float z=0.0f);

	float x;
	float y;
	float z;
};

OPEN_NAMESPACE(Math);

CLOSE_NAMESPACE(Math);

CLOSE_NAMESPACE(Firestorm);

#endif
