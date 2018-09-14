
#ifndef LIBHARNESSED_H_
#define LIBHARNESSED_H_
#pragma once

#include <libCore/libCore.h>

#include "TestHarness.h"
#include "TestCase.h"

OPEN_NAMESPACE(Firestorm);
struct libHarnessed : public Library<libHarnessed>
{
	FIRE_LIBRARY(libHarnessed);
private:
	static void Initialize(int ac, char** av);
};
CLOSE_NAMESPACE(Firestorm);

#endif
