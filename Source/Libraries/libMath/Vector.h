////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Vector
//
//  3D/2D Vector classes and their operations.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
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
	Vector2();
	Vector2(float x, float y);
	explicit Vector2(const Vector2& v);
	explicit Vector2(const Vector3& v);
	explicit Vector2(const Vector4& v);

	Vector2 operator*(float scalar)
	{
		return Vector2(x * scalar, y * scalar);
	}

	Vector2& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	float x, y;
};

class Vector3
{
public:
	Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
	Vector3(const Vector2& v2d, float z = 0.0f);
	explicit Vector3(const Vector4& v2d);

	Vector3 operator*(float scalar)
	{
		return Vector3(x * scalar, y*scalar, z*scalar);
	}

	Vector3& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	float x, y, z;
};

class Vector4
{
public:
	Vector4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w=0.0f);
	Vector4(const Vector2& v, float z = 0.0f, float w = 0.0f);
	Vector4(const Vector3& v, float w = 0.0f);

	Vector4 operator*(float scalar)
	{
		return Vector4(x * scalar, y*scalar, z*scalar, w*scalar);
	}

	Vector4& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}

	float x, y, z, w;
};

CLOSE_NAMESPACE(Firestorm);

#endif
