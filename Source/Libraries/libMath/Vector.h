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

class Vector2;
class Vector3;
class Vector4;

class Vector2
{
public:
	Vector2(float x = 0.0f, float y = 0.0f);
	explicit Vector2(const Vector3& v);
	explicit Vector2(const Vector4& v);

	float x, y;
};

class Vector3
{
public:
	Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
	Vector3(const Vector2& v2d, float z = 0.0f);
	explicit Vector3(const Vector4& v2d);

	float x, y, z;
};

class Vector4
{
public:
	Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w=0.0f);
	Vector4(const Vector2& v, float z = 0.0f, float w = 0.0f);
	Vector4(const Vector3& v, float w = 0.0f);

	float x, y, z, w;
};

OPEN_NAMESPACE(Math);

CLOSE_NAMESPACE(Math);

CLOSE_NAMESPACE(Firestorm);

#endif
