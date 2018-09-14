
#ifndef LIBSCENE_H_
#define LIBSCENE_H_
#pragma once

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);

struct libScene : public Library<libScene>
{
	FIRE_LIBRARY(libScene);
private:
	static void Initialize(int ac, char** av);
};

CLOSE_NAMESPACE(Firestorm);

#endif
