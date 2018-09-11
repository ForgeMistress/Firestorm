
#ifndef LIBSERIAL_H_
#define LIBSERIAL_H_
#pragma once

#include <libJson/libJson.h>

OPEN_NAMESPACE(Firestorm);

struct libSerial
{
	static void Initialize(int ac, char** av);

	static Json::Value Convert(const Mirror::Variant& var);
	static Mirror::Variant Convert(const Json::Value& var);
};

CLOSE_NAMESPACE(Firestorm);

#endif
