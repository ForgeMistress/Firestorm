////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IRefCounted
//
//  Base class for anything that wants to be used by RefPtr.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_IREFCOUNTED_H_
#define LIBCORE_IREFCOUNTED_H_
#pragma once

OPEN_NAMESPACE(Firestorm);


class IRefCounted
{
	template <class T> friend class RefPtr;
	template <class T> friend class WeakPtr;
public:
	IRefCounted() {}
	virtual ~IRefCounted() {}

	size_t AddRef() { return ++_refCount; }
	size_t DelRef() { return --_refCount; }
	size_t GetRefCount() const { return _refCount; }

private:
	size_t _refCount{ 0 };
};


CLOSE_NAMESPACE(Firestorm);

#endif