////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PrimitiveTopology
//
//  Enumeration for all of the primitive topologies that the rendering system supports.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBSCENE_PRIMITIVETOPOLOGY_H_
#define LIBSCENE_PRIMITIVETOPOLOGY_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

enum struct PrimitiveTopology : uint8_t
{
	kPOINT_LIST = 0,
	kLINE_LIST = 1,
	kLINE_STRIP = 2,
	kTRIANGLE_LIST = 3,
	kTRIANGLE_STRIP = 4,
	kTRIANGLE_FAN = 5,
	kLINE_LIST_WITH_ADJACENCY = 6,
	kLINE_STRIP_WITH_ADJACENCY = 7,
	kTRIANGLE_LIST_WITH_ADJACENCY = 8,
	kTRIANGLE_STRIP_WITH_ADJACENCY = 9,
	kPATCH_LIST = 10,
};

CLOSE_NAMESPACE(Firestorm);

#endif