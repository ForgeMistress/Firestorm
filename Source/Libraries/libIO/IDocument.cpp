///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IDocument.h
//
//  A document.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "IDocument.h"
OPEN_NAMESPACE(Firestorm);

FIRE_ERRORCODE_DEF(IDocument::ERROR, "an error occurred");
FIRE_ERRORCODE_DEF(IDocument::READ_DOCUMENT_FAILED, "read of a document failed");
FIRE_ERRORCODE_DEF(IDocument::READ_VALUE_FAILED, "read of a value failed");
FIRE_ERRORCODE_DEF(IDocument::FIND_SUBSECTION_FAILED, "attempted search for a subsection failed");
FIRE_ERRORCODE_DEF(IDocument::ENTER_SUBSECTION_FAILED, "attempted entrance to a subsection failed");
FIRE_ERRORCODE_DEF(IDocument::GET_TYPE_FAILED, "retrieval of a type failed");

CLOSE_NAMESPACE(Firestorm);