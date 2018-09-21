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

ErrorCode IDocument::ERROR("an error occurred");
ErrorCode IDocument::READ_DOCUMENT_FAILED("read of a document failed");
ErrorCode IDocument::READ_VALUE_FAILED("read of a value failed");
ErrorCode IDocument::FIND_SUBSECTION_FAILED("attempted search for a subsection failed");
ErrorCode IDocument::ENTER_SUBSECTION_FAILED("attempted entrance to a subsection failed");
ErrorCode IDocument::GET_TYPE_FAILED("retrieval of a type failed");

CLOSE_NAMESPACE(Firestorm);