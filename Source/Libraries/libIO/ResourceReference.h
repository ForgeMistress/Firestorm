///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceReference.h
//
//  Contains \ref ResourceReference.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_RESOURCEREFERENCE_H_
#define LIBIO_RESOURCEREFERENCE_H_
#pragma once

#include "IResourceObject.h"
#include "ResourceLoader.h"
#include <future>
#include <libCore/IRefCounted.h>

OPEN_NAMESPACE(Firestorm);

/**
	\class ResourceReference

	This class instance does not contain any data. Instead, it merely defines a reference to a resource.
 **/
class ResourceReference final : public Mirror::Object,
                                public IRefCounted
{
	FIRE_MIRROR_DECLARE(ResourceReference, Mirror::Object);
public:
	struct Errors
	{
		FIRE_ERRORCODE(UNKNOWN_STATE);        // set when the state is unknown.
		FIRE_ERRORCODE(THERE_IS_NO_ERROR);             // returned when there's no error set.
		FIRE_ERRORCODE(NOT_FINISHED_LOADING); // returned when the resource has not finished loading yet.
	};

	ResourceReference(const String& path = "");
	ResourceReference(ResourceReference&& other);
	virtual ~ResourceReference();

	/**
		Retrieve the path to the resource on disk.
	 **/
	const String& GetResourcePath() const;

	/**
		Retrieve the path to this resource without the filename.
	 **/
	String GetPathTo() const;

	/**
		Retrieve whether or not the file is loaded.
	 **/
	bool IsReady() const;

	/**
		Check whether or not the resource loaded with an error.
	 **/
	bool HasError() const;

	/**
		Check whether or not the ResourceReference is holding a resource. Opposite of HasError
	 **/
	bool HasResource() const;

	template<class Resource_t>
	RefPtr<Resource_t> GetResource() const
	{
		static_assert(std::is_base_of<IResourceObject, Resource_t>::value, "cast type must derive "
                                                                           "from IResourceObject");
		auto resource = GetResource();
		return resource.Upcast<Resource_t>();
	}

	/**
		Retrieve the resource if it is finished loading.
	 **/
	RefPtr<IResourceObject> GetResource() const;

	/**
		Retrieve the error if it is reported that there is an error.

		\note If a call to #ResourceReference::HasError is made before the resource finished loading,
		then this will return #ResourceReference::Errors::NOT_FINISHED_LOADING. This is meant to convey
		that there is no state to really work with yet. IT IS NOT A CRITICAL ERROR! IT IS MERELY A WARNING!
		Functions like #ResourceReference::HasError should only be called once #ResourceReference::IsReady 
		returns true, that way you can know for sure.
	 **/
	Error GetError() const;

	/**
		Release the loaded resource being held by this ResourceReference.
	 **/
	void Release();

private:
	friend class ResourceMgr;
	void SetResourcePath(const String& path);

	// void SetFuture(std::future<Result<RefPtr<IResourceObject>, Error>>&& future);
	void SetResult(const ResourceLoader::LoadResult& result);

	// so much mutable...
	mutable Mutex _lock;
	mutable RefPtr<IResourceObject> _resource;
	mutable Error _error;
	mutable bool _errorSet{ false };
	mutable bool _isReady{ false };

	String _resourcePath;
};

CLOSE_NAMESPACE(Firestorm);

#endif