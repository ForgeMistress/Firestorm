////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Vector
//
//  A vector.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Vector.h"

OPEN_NAMESPACE(Firestorm);

Vector2::Vector2()
: x(0.0f)
, y(0.0f)
{
}

Vector2::Vector2(float x, float y)
: x(x)
, y(y)
{
}

Vector2::Vector2(const Vector2& v)
: x(v.x)
, y(v.y)
{
}

Vector2::Vector2(const Vector3& v)
: x(v.x)
, y(v.y)
{
}

Vector2::Vector2(const Vector4& v)
: x(v.x)
, y(v.y)
{
}

Vector3::Vector3(float x, float y, float z)
: x(x)
, y(y)
, z(z)
{
}

Vector3::Vector3(const Vector4& v)
: x(v.x)
, y(v.y)
, z(v.z)
{
}

Vector3::Vector3(const Vector2& v, float z)
: x(v.x)
, y(v.y)
, z(z)
{
}

Vector4::Vector4(float x, float y, float z, float w)
: x(x)
, y(y)
, z(z)
, w(w)
{
}

Vector4::Vector4(const Vector2& v, float z, float w)
: x(v.x)
, y(v.y)
, z(z)
, w(w)
{
}

Vector4::Vector4(const Vector3& v, float w)
: x(v.x)
, y(v.y)
, z(v.z)
, w(w)
{
}

OPEN_NAMESPACE(Math);

CLOSE_NAMESPACE(Math);

CLOSE_NAMESPACE(Firestorm);
