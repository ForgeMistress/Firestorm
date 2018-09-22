
#ifndef LIBIO_H_
#define LIBIO_H_
#pragma once

#include <libCore/libCore.h>
#include <libCore/Result.h>
#include <libCore/RefPtr.h>

OPEN_NAMESPACE(Firestorm);

struct libIO : public Library<libIO>
{
	FIRE_LIBRARY(libIO);

	static const ErrorCode* INTERNAL_ERROR;

	/**
		Mount a directory at the specified mount point.
	 **/
	static bool Mount(const String& directory, const String& mountpoint);

	/**
		Check if the file exists.
	 **/
	static bool FileExists(const String& filename);

	/**
		Load a file from disk. The operation happens synchronously.
	 **/
	static Result<Vector<char>, Error> LoadFile(const String& filename);

	/**
		Load a file and return the result as a string.
	 **/
	static Result<String, Error> LoadFileString(const String& filename);
	
	/**
		Retrieve all of the files in a given path.
	 **/
	static Vector<String> GetFiles(const String& path);
private:
	static void Initialize(int ac, char** av);
};

CLOSE_NAMESPACE(Firestorm);

#endif
