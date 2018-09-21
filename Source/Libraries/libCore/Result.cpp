///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Result.cpp
//
//  A result of an operation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Result.h"
#include <sstream>

OPEN_NAMESPACE(Firestorm);

Error::Error(const ErrorCode* errorCode, const String& details)
: _code(errorCode)
, _details(details)
{

}

Error::operator String() const
{
	std::stringstream ss;
	ss << "[" << _code->_code << "] ERROR: " << _code->_message;
	if(!_details.empty())
		ss << std::endl << "    DETAILS: " << _details;
	return ss.str();
}

Error::operator uint32_t() const
{
	return _code->_code;
}

bool Error::operator==(const Error& e) const
{
	return _code->_code == e._code->_code;
}

uint32_t ErrorCode::s_errorCodes = 0;

ErrorCode::ErrorCode(const String& message)
: _code(++s_errorCodes)
, _message(message) {}

ErrorCode::operator String() const 
{ 
	return _message; 
}

ErrorCode::operator const String&() const
{ 
	return _message; 
}

ErrorCode::operator uint32_t() const
{ 
	return _code;
}

CLOSE_NAMESPACE(Firestorm);