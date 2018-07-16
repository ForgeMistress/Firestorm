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

OPEN_NAMESPACE(Elf);

/**
	\class ResourceReference

	This class instance does not contain any data. Instead, it merely defines a reference to a resource.
 **/
class ResourceReference : public Mirror::Object
{
	ELF_MIRROR_DECLARE(ResourceReference, Mirror::Object);
public:
	ResourceReference();
	virtual ~ResourceReference();

protected:
private:
};

CLOSE_NAMESPACE(Elf);

#endif