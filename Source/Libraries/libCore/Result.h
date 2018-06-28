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

#include <functional>
#include "Expected.h"

OPEN_NAMESPACE(Elf);

class Error
{
public:
	Error(int code, const String& message)
	: Code(code)
	, Message(message) {}

	explicit operator String() const { return Message; }
	explicit operator const String&() const { return Message; }
	explicit operator int() const { return Code; }
	template<class Enum_t> operator Enum_t() const { return static_cast<Enum_t>(Code); }

	bool operator==(const int& e) const { return Code == e; }
	bool operator==(const Error& e) const { return Code == e.Code; }

	int     Code;
	String  Message;
};

template <class Ex> using Result = tl::expected<Ex, Error>;


template <class Expected_t>
static tl::expected<Expected_t, Error> result(Expected_t&& e)
{
	return tl::expected<Expected_t, Error>(e);
}

static tl::expected<void, Error> result()
{
	return tl::expected<void, Error>();
}

static Error error(int e, const String& msg)
{
	return Error(e, msg);
}


	/**
		Global OK result.
	 **/
	// static ResultCode OK;

	/**
		Global generic ERROR result. Prefer custom results over this one.
	 **/
	// static ResultCode ERROR;

	/**
		Retrieve the string message held by this Result.
	 **/
	//const String& GetMessage() const;

	/**
		Check on whether or not the Result defines an error.
	 **/
	//bool IsOK() const;

	/**
		Inverse of IsOK.
	 **/
	//bool IsNotOK() const;

	/** 
		Equivalence Operator 
	 **/
	//bool operator==(ResultCode result) const;

	/**
		Casting Operators
	 **/
	//operator const String&() { return m_message; }
	//operator String()        { return m_message; }
	//operator const char*()   { return m_message.c_str(); }


	//explicit Result(const String& message);

//private:
	//String m_message;
//};

CLOSE_NAMESPACE(Elf);
#endif