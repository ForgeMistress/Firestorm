
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
	static bool Mount(const string& directory, const string& mountpoint);

	/**
		Check if the file exists.
	 **/
	static bool FileExists(const string& filename);

	/**
		Load a file from disk. The operation happens synchronously.
	 **/
	static Result<vector<char>, Error> LoadFile(const string& filename);

	/**
		Load a file and return the result as a string.
	 **/
	static Result<string, Error> LoadFileString(const string& filename);
	
	/**
		Retrieve all of the files in a given path.
	 **/
	static vector<string> GetFiles(const string& path);
private:
	static void Initialize(int ac, char** av);
};

CLOSE_NAMESPACE(Firestorm);

#endif
