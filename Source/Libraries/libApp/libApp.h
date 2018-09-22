#ifndef LIBAPP_H_
#define LIBAPP_H_
#pragma once

#include <libCore/LibraryRegistrar.h>
#include <libMirror/EventDispatcher.h>
#include <libMirror/Object.h>

#include <libCore/ArgParser.h>

#include "Surface.h"

OPEN_NAMESPACE(Firestorm);

struct libApp : public Library<libApp>
{
	FIRE_LIBRARY(libApp);

	static void Initialize(int ac, char** av);
};

CLOSE_NAMESPACE(Firestorm);

#endif
