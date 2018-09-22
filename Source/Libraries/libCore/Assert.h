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

#include <ostream>
#include <sstream>

OPEN_NAMESPACE(Firestorm);


class AssertionException : public std::exception
{
public:
	class StreamFormatter
	{
	public:
		operator std::string() const;

		template<class T>
		StreamFormatter& operator<<(const T& value)
		{
			_stream << value;
			return *this;
		}

	private:
		std::ostringstream _stream;
	};


	AssertionException(const char* expression, const char* file, int line, const std::string& message);

	virtual const char* what() const throw();

	const char*         Expression() const throw();
	const char*         File()       const throw();
	int                 Line()       const throw();
	const char*         Message()    const throw();
	const char*         Report()     const throw();

private:
	const char* _expression;
	const char* _file;
	int         _line;
	std::string _message;
	std::string _report;
};

#define FIRE_ASSERT_MSG(EXPRESSION, MESSAGE)    \
	if(!(EXPRESSION))							\
	{											\
		AssertionException::StreamFormatter f;  \
		f << MESSAGE;                           \
		throw AssertionException(				\
			#EXPRESSION,						\
			__FILE__,							\
			__LINE__,							\
			(std::string)f);                    \
	}

#define FIRE_ASSERT(EXPRESSION)                \
	if(!(EXPRESSION))						   \
	{										   \
		AssertionException::StreamFormatter f; \
		f << "an assertion was thrown";        \
		throw AssertionException(			   \
			#EXPRESSION,					   \
			__FILE__,						   \
			__LINE__,						   \
			(std::string)f);                   \
	}

CLOSE_NAMESPACE(Firestorm);

#endif