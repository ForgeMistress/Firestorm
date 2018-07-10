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

	template<class Enum_t> 
	Enum_t ToEnum() const { 
		return static_cast<Enum_t>(Code); 
	}

	bool operator==(const int& e) const { return Code == e; }
	bool operator==(const Error& e) const { return Code == e.Code; }

	int     Code;
	String  Message;
};

template<typename Ex_t>
struct _ResultTemplate
{
	typedef typename std::remove_reference<Ex_t>::type  expected_type;
	typedef typename tl::expected<expected_type, Error> result_type;
};

template <typename Ex_t, typename UEx_t> using Result =
	tl::expected<Ex_t, UEx_t>;

/*template <class Ex_t>
static Result<Ex_t, Error> result(Ex_t&& e)
{
	return Result<Ex_t, Error>(e);
}

static Result<void, Error> void_result()
{
	return Result<void, Error>();
}

static Result<void, Error> void_error(int e, const String& msg)
{
	return Result<void, Error>(tl::unexpect, Error(e, msg));
}

template<typename Ex_t>
static Result<Ex_t, Error> error(int e, const String& msg)
{
	return Result<void, Error>(tl::unexpect, Error(e, msg));
}

OPEN_NAMESPACE(Result);
template<class Expected_t>
static bool IsOK(const Result<Expected_t, Error>& result)
{
	return result.has_value();
}

template<class Expected_t>
static bool IsNotOK(const Result<Expected_t, error>& result)
{
	return !result.has_value();
}

CLOSE_NAMESPACE(Result);*/

CLOSE_NAMESPACE(Elf);
#endif