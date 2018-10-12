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

struct UUIDType {}; // void* doesn't play nice with the SOA.
using UUID = UUIDType*;

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
	vector<void*> _trackedPages;
};


CLOSE_NAMESPACE(Firestorm);

#endif
