
#ifndef LIBIO_H_
#define LIBIO_H_
#pragma once

#include <libCore/libCore.h>
#include <libCore/Result.h>

OPEN_NAMESPACE(Firestorm);

struct libIO : public Library<libIO>
{
	FIRE_LIBRARY(libIO);

	/**
		Mount a directory at the specified mount point.
	 **/
	static bool Mount(const String& directory, const String& mountpoint);

	/**
		Load a file from disk. The operation happens synchronously.
	 **/
	static Result<Vector<char>, Error> LoadFile(const String& filename);

	static Result<String, Error> LoadFileString(const String& filename);

private:
	static void Initialize(int ac, char** av);
};

CLOSE_NAMESPACE(Firestorm);

#endif
