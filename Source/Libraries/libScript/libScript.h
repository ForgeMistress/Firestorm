
#ifndef LIBSCRIPT_H_
#define LIBSCRIPT_H_
#pragma once

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);

struct libScript : public Library<libScript>
{
	FIRE_LIBRARY(libScript);
private:
	static void Initialize(int ac, char** av);
};
CLOSE_NAMESPACE(Firestorm);

#endif
