///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  FileIOMgr.h
//
//  
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_FILEIOMGR_H_
#define LIBIO_FILEIOMGR_H_
#pragma once

#include <libCore/Expected.h>
#include <libCore/Result.h>
#include <libCore/SynchronizedQueue.h>
#include <libMirror/EventDispatcher.h>
#include <libMirror/ObjectMaker.h>
#include <libCore/libCore.h>
#include "ResourceReference.h"

#include "IResourceObject.h"

OPEN_NAMESPACE(Firestorm);


struct ResourceIOErrors
{
	enum Errors : char
	{
		//< Can't use the default loader dumbass.
		kDefaultLoader,

		//< Return this error when the file could not be found.
		kFileNotFound,

		//< Return this if the file fails to load for some reason.
		kFileRead,

		//< Return this error when there's an error with parsing.
		kParsingException,

		kProcessingException
	};
};

using ResourceLoadResult_t = Result<RefPtr<IResourceObject>, Error>;
using LoadFunctor_t = Function<ResourceLoadResult_t(const ResourceReference&)>;

struct ResourceLoader
{
	ResourceLoader(const ResourceReference& ref);
	ResourceLoadResult_t operator()();
};

/**
	Specialize this to provide the data for the resource loader.
 **/
template<class T>
struct ResourceTraits
{
	using type = T;
	using loader = ResourceLoader;
};

/**
	\class FileIOMgr

	Manages the asynchronous loading of 

	https://github.com/embeddedartistry/embedded-resources/blob/master/examples/cpp/dispatch.cpp
 **/
class FileIOMgr final
{
	typedef std::pair<const ResourceReference&, LoadFunctor_t> LoadOp_t;
public:
	FileIOMgr(ObjectMaker& objMaker);
	~FileIOMgr();

	template <class ResourceType_t>
	void Load(const ResourceReference& ref)
	{
		Load(std::make_pair(ref,_objMaker.Make<ResourceTraits<ResourceType_t>::loader>()));
	}
	void Load(LoadOp_t loadFunctor);

	void Shutdown();

private:
	static const char _numThreads{ 2 };

	String _name;
	Mutex _lock;

	Thread _threads[_numThreads];

	std::queue<LoadOp_t> _queue;
	std::condition_variable _cv;
	bool _quit{ false };

	ObjectMaker& _objMaker;

	void ThreadRun();
};

CLOSE_NAMESPACE(Firestorm);
#endif