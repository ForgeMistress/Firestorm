////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AABB.cpp
//
//  An Axis Aligned Bounding Box.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AABB.h"

OPEN_NAMESPACE(Elf);

AABB_2D::AABB_2D()
: _top(0)
, _left(0)
, _bottom(0.1f)
, _right(0.1f)
{
}

AABB_2D::AABB_2D(float top, float left, float bottom, float right)
: _top(top)
, _left(left)
, _bottom(bottom)
, _right(right)
{
}

AABB_3D::AABB_3D()
	: _minx(0)
	, _miny(0)
	, _minz(0)
	, _maxx(0.1f)
	, _maxy(0.1f)
	, _maxz(0.1f)
{
}

AABB_3D::AABB_3D(float minx, float miny, float minz, float maxx, float maxy, float maxz)
	: _minx(minx)
	, _miny(miny)
	, _minz(minz)
	, _maxx(maxx)
	, _maxy(maxy)
	, _maxz(maxz)
{
}

OPEN_NAMESPACE(Math);

bool Intersects(const AABB_2D& lhs, const AABB_2D& rhs)
{
	return
		!(rhs._left > lhs._right
		|| rhs._right < lhs._left
		|| rhs._top < lhs._bottom
		|| rhs._bottom > lhs._top);
}

bool Intersects(const AABB_3D& lhs, const AABB_3D& rhs)
{

}

CLOSE_NAMESPACE(Math);

CLOSE_NAMESPACE(Elf);
