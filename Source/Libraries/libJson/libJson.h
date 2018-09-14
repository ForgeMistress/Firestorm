#ifndef LIBJSON_H_
#define LIBJSON_H_
#pragma once

#include <libCore/libCore.h>
#include <json/value.h>

OPEN_NAMESPACE(Firestorm);

struct libJson : public Library<libJson>
{
	FIRE_LIBRARY(libJson);
private:
	static void Initialize(int ac, char** av);
};

CLOSE_NAMESPACE(Firestorm);

#endif
