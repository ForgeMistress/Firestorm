
#ifndef LIBHARNESSED_H_
#define LIBHARNESSED_H_
#pragma once

#ifdef LIB_HARNESSED
	#error "libHarnessed.h already included"
#endif

#define LIB_HARNESSED

#include "TestHarness.h"
#include "TestCase.h"

#endif
