
#ifndef LIBFIRESTORM_H_
#define LIBFIRESTORM_H_
#pragma once

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);

struct libFirestorm : public Library<libFirestorm>
{
	FIRE_LIBRARY(libFirestorm);
private:
	static void Initialize(int ac, char** av);
};

CLOSE_NAMESPACE(Firestorm);

#endif
