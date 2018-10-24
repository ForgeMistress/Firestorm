///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceLoader
//
//  The base class for a resource loader.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCELOADER_H_
#define LIBIO_RESOURCELOADER_H_
#pragma once

#include "IResourceObject.h"
#include <libCore/Result.h>

#define FIRE_LOAD_SUCCESS ::Firestorm::LoadResult()

#define FIRE_LOAD_FAIL( ERROR_CODE, DETAILS ) \
	::Firestorm::LoadResult(  \
		::Firestorm::Error(                   \
			ERROR_CODE,                       \
			DETAILS,                          \
			__FILE__,                         \
			__LINE__                          \
		)                                     \
	)

OPEN_NAMESPACE(Firestorm);

class ResourceReference;
class ResourceMgr;

struct LoadResult
{
public:
	LoadResult();
	LoadResult(const LoadResult& other);
	LoadResult(LoadResult&& other);
	LoadResult(const Error& error);

	LoadResult& operator=(LoadResult&& other);
	LoadResult& operator=(const LoadResult& other);

	Error GetError() const;

	bool HasError() const;
	bool IsOk() const { return !HasError(); }

private:
	Error _error;
};

//class ResourceLoader
//{
//public:
//	ResourceLoader();
//	virtual ~ResourceLoader();
//
//	virtual LoadResult Load(ResourceMgr* resourceMgr, const ResourceReference& ref);
//};

CLOSE_NAMESPACE(Firestorm);

#endif
