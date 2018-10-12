#ifndef LIBCORE_H_
#define LIBCORE_H_
#pragma once

#define OPEN_NAMESPACE(ns) namespace ns {
#define CLOSE_NAMESPACE(ns) } // namespace ns

#include "Common.h"
#include "LibraryRegistrar.h"
#include "Expected.h"

OPEN_NAMESPACE(Firestorm);

using std::cout;
using std::endl;

#ifdef FIRE_FINAL
#define fire_new(TYPE, ...)     ::Firestorm::libCore::New<TYPE>(__VA_ARGS__)
#define fire_alloc(SIZE)        ::Firestorm::libCore::Alloc(SIZE)
#define fire_delete(TYPE, PTR)  ::Firestorm::libCore::Delete<TYPE>(PTR)
#define fire_free(PTR)          ::Firestorm::libCore::Free(PTR)
#else
#define fire_new(TYPE, ...)    ::Firestorm::libCore::New<TYPE>(__FILE__, __LINE__, __VA_ARGS__)
#define fire_alloc(SIZE)       ::Firestorm::libCore::Alloc(SIZE, __FILE__, __LINE__)
#define fire_delete(TYPE, PTR) ::Firestorm::libCore::Delete<TYPE>(PTR)
#define fire_free(PTR)         ::Firestorm::libCore::Free(PTR)
#endif

struct libCore : public Library<libCore>
{
	FIRE_LIBRARY(libCore);

	static void SetThreadName(Thread& thread, const string& name);

#ifdef FIRE_FINAL
	template<class T>
	static T* Alloc(size_t numItems)
	{
		return (T*)Alloc(numItems * sizeof(T));
	}

	template<class T, class... Args>
	static T* New(Args&&... args)
	{
		void* ptr = Alloc(sizeof(T));
		new(ptr) T(std::forward<Args>(args)...);
		return ptr;
	}

	static void* Alloc(size_t sizeInBytes);
#else
	template<class T>
	static T* Alloc(size_t numItems, const char* file, size_t line)
	{
		return (T*)Alloc(numItems * sizeof(T), file, line);
	}

	template<class T, class... Args>
	static T* New(const char* file, size_t line, Args&&... args)
	{
		void* ptr = Alloc(sizeof(T), file, line);
		new(ptr) T(std::forward<Args>(args)...);
		return ptr;
	}

	static void* Alloc(size_t sizeInBytes, const char* file, size_t line);
#endif

	static void Free(void* block);

	template<class T>
	static void Delete(T* ptr)
	{
		ptr->~T();
		Free(ptr);
	}

	static void ReportMemoryLeaks();

private:
	static void Initialize(int ac, char** av);

};

extern vector<string> SplitString(const string& str, char delim);





template <class Class_t, class Arg_t>
function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&), Class_t* instance)
{
	function<void(const Arg_t&)> f(funcPointer, instance, eastl::function::)
	return eastl::bind(funcPointer, instance, std::placeholders::_1);
}

template <class Class_t, class Arg_t>
function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&), const Class_t* instance)
{
	return eastl::bind(funcPointer, const_cast<Class_t*>(instance), std::placeholders::_1);
}

template <class Class_t, class Arg_t>
function<void(const Arg_t&)> WrapFn(void (Class_t::*funcPointer)(const Arg_t&) const, const Class_t* instance)
{
	return std::bind(funcPointer, const_cast<Class_t*>(instance), std::placeholders::_1);
}

template <class Arg_t>
function<void(const Arg_t&)> WrapFn(void(*funcPointer)(const Arg_t&))
{
	return function<void(const Arg_t&)>(funcPointer);
}

CLOSE_NAMESPACE(Firestorm);

/**
	Use this to flag a variable as unused in a function.
 **/
#define FIRE_UNUSED_VARIABLE(var) ((void)var)

#endif