
#ifndef LIBIO_H_
#define LIBIO_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

struct libIO
{
	static void Initialize(int ac, char** av);

	/**
		Mount a directory at the specified mount point.
	 **/
	static bool Mount(const String& directory, const String& mountpoint);

	/**
		Load a file from disk. The operation happens synchronously.
	 **/
	static Vector<char> LoadFile(const String& filename);

	static String LoadFileString(const String& filename);
};

CLOSE_NAMESPACE(Firestorm);

#endif
