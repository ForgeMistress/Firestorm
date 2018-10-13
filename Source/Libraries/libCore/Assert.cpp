///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Assert
//
//  You know what this does.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Assert.h"

#include "Logger.h"

OPEN_NAMESPACE(Firestorm);


AssertionException::AssertionException(const eastl::string& message)
: _expression("<force>")
, _file("<force>")
, _line(0)
, _message(message)
{
	Format();
}

AssertionException::AssertionException(const char*   expression,
                                       const char*   file, 
                                       int           line,
                                       const eastl::string& message)
: _expression(expression)
, _file(file)
, _line(line)
, _message(message)
{
	Format();
}

void AssertionException::Format() throw()
{
	//std::ostringstream ostream;
	string ostream;
	if(!_message.empty())
	{
		ostream.append_sprintf("%s: ", _message.c_str());
	}
	string expr(_expression);

	if(expr == "false" || expr == "0" || expr == "FALSE")
	{
		ostream.append_sprintf("Unreachable Code Assertion");
	}
	else
	{
		ostream.append_sprintf("Assertion '%s'", _expression);
	}
	ostream.append_sprintf(" failed in file '%s' line# %d", _file, _line);
	_report = ostream;
}


const char* AssertionException::what() const throw()
{
	return _report.c_str();
}

const char* AssertionException::Expression() const throw()
{
	return _expression;
}

const char* AssertionException::File() const throw()
{
	return _file;
}

int AssertionException::Line() const throw()
{
	return _line;
}

const char* AssertionException::Message() const throw()
{
	return _message.c_str();
}

const char* AssertionException::Report() const throw()
{
	return _report.c_str();
}

CLOSE_NAMESPACE(Firestorm);