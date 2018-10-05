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


AssertionException::StreamFormatter::operator std::string() const
{
	return _stream.str();
}

AssertionException::AssertionException(const std::string& message)
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
                                       const std::string& message)
: _expression(expression)
, _file(file)
, _line(line)
, _message(message)
{
	Format();
}

void AssertionException::Format() throw()
{
	std::ostringstream ostream;
	if(!_message.empty())
	{
		ostream << _message << ": ";
	}
	String expr(_expression);

	if(expr == "false" || expr == "0" || expr == "FALSE")
	{
		ostream << "Unreachable Code Assertion";
	}
	else
	{
		ostream << "Assertion '" << _expression << "'";
	}
	ostream << " failed in file '" << _file << "' line# " << _line;
	_report = ostream.str();

	FIRE_LOG_ERROR(_report.c_str());
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