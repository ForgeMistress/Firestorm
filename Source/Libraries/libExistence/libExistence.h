
#ifndef LIB_EXISTENCE_H_
#define LIB_EXISTENCE_H_
#pragma once

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);
struct libExistence : public Library<libExistence>
{
	FIRE_LIBRARY(libExistence);
private:
	static void Initialize(int ac, char** av);
};
CLOSE_NAMESPACE(Firestorm);

#endif
