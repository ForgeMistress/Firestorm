///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  LibraryRegistrar
//
//  Base class that handles the registration of the C++ classes and only allows the registration function to run once.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_LIBRARYREGISTRAR_H_
#define LIBCORE_LIBRARYREGISTRAR_H_
#pragma once

#define FIRE_LIBRARY(LIBTYPE)                   \
	template <class T> friend struct Library;   \
	static constexpr char* s_libName = #LIBTYPE

OPEN_NAMESPACE(Firestorm);

template <class T>
struct Library
{
	using Lib = Library<T>;
	static void Initialize(int ac, char** av);

	template <class U>
	static void RegisterReflection();
};

template<class T>
void Library<T>::Initialize(int ac, char** av)
{
	static bool _isInitialized = false;
	if(_isInitialized)
		return;
	_isInitialized = true;
	std::cout << ":: Initializing " << T::s_libName << std::endl << std::flush;
	T::Initialize(ac, av);
}

template<class T>
template<class U>
void Library<T>::RegisterReflection()
{
	U::RegisterReflection();
	Mirror::Type t = Mirror::Type::get<U>();
	FIRE_LOG_DEBUG("        :: Size of %s = %d ((%d))", t.get_name(), t.get_sizeof(), sizeof(U));
}

CLOSE_NAMESPACE(Firestorm);

#endif