////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AABB.h
//
//  An Axis Aligned Bounding Box.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMIRROR_OBJECT_H_
#define LIBMIRROR_OBJECT_H_
#pragma once

OPEN_NAMESPACE(Elf);

class AABB2f : public Mirror::Object
{
    MIRROR_DECLARE(AABB2f);
public:
    AABB();
    AABB(float minx, float miny, float maxx, float maxy);

private:

};


CLOSE_NAMESPACE(Elf);

#endif