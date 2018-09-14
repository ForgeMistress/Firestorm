#ifndef LIBMIRROR_H_
#define LIBMIRROR_H_
#pragma once

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);
struct libMirror : public Library<libMirror>
{
	FIRE_LIBRARY(libMirror);
private:
	static void Initialize(int ac, char** av);
};
CLOSE_NAMESPACE(Firestorm);

#endif
