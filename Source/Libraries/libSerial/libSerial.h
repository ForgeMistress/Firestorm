
#ifndef LIBSERIAL_H_
#define LIBSERIAL_H_
#pragma once

#include <libCore/libCore.h>
#include <libJson/libJson.h>

OPEN_NAMESPACE(Firestorm);

struct libSerial : public Library<libSerial>
{
	FIRE_LIBRARY(libSerial);

	static Json::Value Convert(const Mirror::Variant& var);
	static Mirror::Variant Convert(const Json::Value& var);
private:
	static void Initialize(int ac, char** av);
};

CLOSE_NAMESPACE(Firestorm);

#endif
