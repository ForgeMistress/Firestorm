#pragma once
#ifdef LIB_HARNESSED
#error "libHarnessed.h already included"
#endif

#define LIB_HARNESSED
#include <libCore.h>

#include <libHarnessed/TestHarness.h>
#include <libHarnessed/TestCase.h>