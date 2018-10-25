////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AABB.h
//
//  An Axis Aligned Bounding Box.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMATH_AABB_H_
#define LIBMATH_AABB_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

class AABB_2D
{
	FIRE_MIRROR_DECLARE(AABB_2D);
public:
	AABB_2D();
	AABB_2D(float top, float left, float bottom, float right);

	float _top;
	float _left;
	float _bottom;
	float _right;
};

class AABB_3D
{
	FIRE_MIRROR_DECLARE(AABB_3D);
public:
	AABB_3D();
	AABB_3D(float minx, float miny, float minz, float maxx, float maxy, float maxz);

	float _minx;
	float _miny;
	float _minz;
	float _maxx;
	float _maxy;
	float _maxz;
};

OPEN_NAMESPACE(Math);

static bool Intersects(const AABB_2D& lhs, const AABB_2D& rhs);
static bool Intersects(const AABB_3D& lhs, const AABB_3D& rhs);

CLOSE_NAMESPACE(Math);
CLOSE_NAMESPACE(Firestorm);

#endif
