#ifndef LIBAPP_H_
#define LIBAPP_H_
#pragma once

#include <libMirror/EventDispatcher.h>
#include <libMirror/Object.h>

#include "Surface.h"

namespace Firestorm
{
	struct libApp
	{
		static void Initialize(int ac, char** av);
	};
}

#endif
