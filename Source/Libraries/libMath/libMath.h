
#ifndef LIBMATH_H_
#define LIBMATH_H_
#pragma once

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);
struct libMath : public Library<libMath>
{
	FIRE_LIBRARY(libMath);
private:
	static void Initialize(int ac, char** av);
};
CLOSE_NAMESPACE(Firestorm);

#endif
