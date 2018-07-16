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
	Enum_t ToEnum() const
	{ 
		return static_cast<Enum_t>(Code); 
	}

	bool operator==(const int& e) const { return Code == e; }
	bool operator==(const Error& e) const { return Code == e.Code; }

	int     Code;
	String  Message;
};

template <typename Expected_t, typename Unexpected_t> using Result =
	tl::expected<Expected_t, Unexpected_t>;

#define ELF_ERROR( CODE, ERROR_STRING ) \
	tl::make_unexpected(Error( CODE, ERROR_STRING ))

CLOSE_NAMESPACE(Elf);
#endif