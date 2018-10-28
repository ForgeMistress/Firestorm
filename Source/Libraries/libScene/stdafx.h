// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef LIBSCENE_STDAFX_H_
#define LIBSCENE_STDAFX_H_
#pragma once

#include <libCore/libCore.h>
#include <libCore/Object.h>
#include <libCore/MirrorMacros.h>

#include <libIO/libIO.h>
#include <libIO/IResourceObject.h>
#include <libIO/ResourceMgr.h>
#include <libIO/ResourceCache.h>

#include <libMath/Vector.h>
#include <libMath/Matrix.h>
#include <libMath/Quaternion.h>

#include <json/json.h>

OPEN_NAMESPACE(Firestorm);

template<class T>
using RSPtr = eastl::shared_ptr<T>;

CLOSE_NAMESPACE(Firestorm);

#endif
