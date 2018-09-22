///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceIOErrors
//
//  Holder for errors that can be returned by a ResourceLoader.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceIOErrors.h"

OPEN_NAMESPACE(Firestorm);

FIRE_ERRORCODE_DEF(ResourceIOErrors::DEFAULT_LOADER, "define a new loader for this type");
FIRE_ERRORCODE_DEF(ResourceIOErrors::FILE_NOT_FOUND_ERROR, "file was not found");
FIRE_ERRORCODE_DEF(ResourceIOErrors::FILE_READ_ERROR, "file could not be read");
FIRE_ERRORCODE_DEF(ResourceIOErrors::PARSING_ERROR, "there was an error while parsing the data in file");
FIRE_ERRORCODE_DEF(ResourceIOErrors::PROCESSING_ERROR, "there was an error while processing the file");

CLOSE_NAMESPACE(Firestorm);
