////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Constants
//
//  Defines constants used by the rest of the math lib as well as other utility functions.
//  This code was borrowed from the Horde3D project.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBMATH_CONSTANTS_H_
#define LIBMATH_CONSTANTS_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

// Constants
namespace Constants
{
	const unsigned int MaxUInt32 = 0xFFFFFFFF;
	const int MinInt32 = 0x80000000;
	const int MaxInt32 = 0x7FFFFFFF;
	const float MaxFloat = 3.402823466e+38F;
	const float MinPosFloat = 1.175494351e-38F;

	const float Pi = 3.141592654f;
	const float TwoPi = 6.283185307f;
	const float PiHalf = 1.570796327f;

	const float Epsilon = 0.000001f;
	const float ZeroEpsilon = 32.0f * MinPosFloat;  // Very small epsilon for checking against 0.0f
	#ifdef __GNUC__
	const float NaN = __builtin_nanf("");
	#else
	const float NaN = *(float *)&MaxUInt32;
	#endif

	enum NoInitHint
	{
		NO_INIT
	};
};

static inline float DegToRad( float f )
{
	return f * 0.017453293f;
}

static inline float RadToDeg( float f )
{
	return f * 57.29577951f;
}

CLOSE_NAMESPACE(Firestorm);

#endif
