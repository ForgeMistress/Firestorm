
#ifndef LIBUI_H_
#define LIBUI_H_
#pragma once

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);
struct libUI : public Library<libUI>
{
	FIRE_LIBRARY(libUI);
private:
	static void Initialize(int ac, char** av);
};
CLOSE_NAMESPACE(Firestormn);

#endif
