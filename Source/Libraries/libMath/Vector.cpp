////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Vector
//
//  A vector.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Vector.h"

OPEN_NAMESPACE(Firestorm);

Vector2::Vector2(float x, float y)
{
	f[0] = x;
	f[1] = y;
}
Vector2::Vector2(const Vector3& v)
{
	f[0] = v.f[0];
	f[1] = v.f[1];
}
Vector2::Vector2(const Vector4& v)
{
	f[0] = v.f[0];
	f[1] = v.f[1];
}

Vector3::Vector3(float x, float y, float z)
{
	f[0] = x;
	f[1] = y;
	f[2] = z;
}

Vector3::Vector3(const Vector4& v)
{
	f[0] = v.f[0];
	f[1] = v.f[1];
	f[2] = v.f[2];
}

Vector3::Vector3(const Vector2& v, float z)
{
	f[0] = v.f[0];
	f[1] = v.f[1];
	f[2] = z;
}

Vector4::Vector4(float x, float y, float z, float w)
{
	f[0] = x;
	f[1] = y;
	f[2] = z;
	f[3] = w;
}

Vector4::Vector4(const Vector2& v, float z, float w)
{
	f[0] = v.f[0];
	f[1] = v.f[1];
	f[2] = z;
	f[3] = w;
}

Vector4::Vector4(const Vector3& v, float w)
{
	f[0] = v.f[0];
	f[1] = v.f[1];
	f[2] = v.f[2];
	f[3] = w;
}

OPEN_NAMESPACE(Math);

CLOSE_NAMESPACE(Math);

CLOSE_NAMESPACE(Firestorm);
