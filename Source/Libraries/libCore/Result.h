///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Result.h
//
//  A result of an operation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_RESULT_H_
#define LIBCORE_RESULT_H_
#pragma once

OPEN_NAMESPACE(Elf);

class Result;
typedef const Result& ResultCode;

/**
 *  Result
 *
 *  Result holds the Result of an operation. It is intended to be a cached error code that you can store within
 *  the class definition.
 *
 *  For example:
 *
 *  // Foo.h
 *  class Foo
 *  {
 *  public:
 *      static const Result& FOO_ERROR;
 *  };
 *
 *  // Foo.cpp
 *  #include "stdafx.h"
 *  #include "Foo.h"
 *
 *  const Result& Foo::FOO_ERROR = Result::MSG("Some error that Foo can return in a function.");
 **/
class Result
{
public:
	/**
		Global OK result.
	 **/
	static ResultCode OK;

	/**
		Global generic ERROR result. Prefer custom results over this one.
	 **/
	static ResultCode ERROR;

	/**
		Either construct a new Result or return an existing one.
	 **/
	static Result MSG(const string& message);

	/**
		Retrieve the string message held by this Result.
	 **/
	const string& GetMessage() const;

	/**
		Check on whether or not the Result defines an error.
	 **/
	bool IsOK() const;

	/**
		Inverse of IsOK.
	 **/
	bool IsNotOK() const;

	/** 
		Equivalence Operator 
	 **/
	bool operator==(ResultCode result);

	/**
		Casting Operators
	 **/
	operator const string&() { return m_message; }
	operator string()        { return m_message; }
	operator const char*()   { return m_message.c_str(); }

protected:
	Result() {}

	// Only can obtain instance through MSG()
	explicit Result(const string& message);

	~Result() {}

private:
	string m_message;
};

CLOSE_NAMESPACE(Elf);
#endif