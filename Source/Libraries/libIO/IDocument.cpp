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

const ErrorCode* IDocument::ERROR(new ErrorCode("an error occurred"));
const ErrorCode* IDocument::READ_DOCUMENT_FAILED(new ErrorCode("read of a document failed"));
const ErrorCode* IDocument::READ_VALUE_FAILED(new ErrorCode("read of a value failed"));
const ErrorCode* IDocument::FIND_SUBSECTION_FAILED(new ErrorCode("attempted search for a subsection failed"));
const ErrorCode* IDocument::ENTER_SUBSECTION_FAILED(new ErrorCode("attempted entrance to a subsection failed"));
const ErrorCode* IDocument::GET_TYPE_FAILED(new ErrorCode("retrieval of a type failed"));

CLOSE_NAMESPACE(Firestorm);