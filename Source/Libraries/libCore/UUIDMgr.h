///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  UUIDMgr
//
//  Obtain for thyself a unique identifier, one that shall be known and feared throughout the cosmos.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_UUIDMGR_H_
#define LIBCORE_UUIDMGR_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

using UUID = void*;

class UUIDMgr final
{
public:
	UUIDMgr();
	~UUIDMgr();

	UUID Get() const;

private:
	size_t GetSystemPageSize() const;
	void* Alloc() const;
	void Free(void* pageStart);

	size_t _systemPageSize;
	mutable void* _uuidBlock{ nullptr };
	mutable size_t _numIDsUsed{ 0 };
	Vector<void*> _trackedPages;
};


CLOSE_NAMESPACE(Firestorm);

#endif
