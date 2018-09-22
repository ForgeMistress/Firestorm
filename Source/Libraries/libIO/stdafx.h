// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef LIBIO_STDAFX_H_
#define LIBIO_STDAFX_H_
#pragma once

#include <libCore/libCore.h>
#include <libCore/Result.h>
#include <libCore/Expected.h>
#include <libCore/RefPtr.h>

#include <libMirror/libMirror.h>
#include <libMirror/Object.h>

#include <physfs/physfs.h>

#include <future>
#include <functional>
#include <thread>

OPEN_NAMESPACE(Firestorm);

class IResourceObject;
using ResourceMgrResult = Result<RefPtr<IResourceObject>, Error>;

CLOSE_NAMESPACE(Firestorm);

#endif
