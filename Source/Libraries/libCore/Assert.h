///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Assert
//
//  You know what this does.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_ASSERT_H_
#define LIBCORE_ASSERT_H_
#pragma once

#include <EASTL/string.h>

OPEN_NAMESPACE(Firestorm);


class AssertionException : public std::exception
{
public:
	AssertionException(const eastl::string& message);
	AssertionException(const char* expression, const char* file, int line, const eastl::string& message);

	virtual const char* what() const throw();

	const char*         Expression() const throw();
	const char*         File()       const throw();
	int                 Line()       const throw();
	const char*         Message()    const throw();
	const char*         Report()     const throw();

private:
	void Format() throw();

	const char*   _expression;
	const char*   _file;
	int           _line;
	eastl::string _message;
	eastl::string _report;
};

#define FIRE_ASSERT_MSG(EXPRESSION, MESSAGE)    \
	if(!(EXPRESSION))							\
	{                                           \
		throw AssertionException(				\
			#EXPRESSION,						\
			__FILE__,							\
			__LINE__,							\
			MESSAGE);                           \
	}

#define FIRE_ASSERT(EXPRESSION)                \
	if(!(EXPRESSION))						   \
	{										   \
		throw AssertionException(			   \
			#EXPRESSION,					   \
			__FILE__,						   \
			__LINE__,						   \
			"an exception was thrown");        \
	}

CLOSE_NAMESPACE(Firestorm);

#endif