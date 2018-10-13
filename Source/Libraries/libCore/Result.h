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

OPEN_NAMESPACE(Firestorm);

class ErrorCode;

class Error
{
public:
	Error();
	Error(const ErrorCode* errorCode, const string& details, const char* file = nullptr, int line = 0);
	Error(const Error& error);
	Error(Error&& other);

	explicit operator string() const;
	explicit operator uint32_t() const;

	Error& operator=(const Error& e);
	bool operator==(const Error& e) const;

	void Set(const ErrorCode* code, const string& details);

	operator bool() const;

	const ErrorCode* GetCode() const { return _code; }

	const char* Format() const;

private:
	const ErrorCode* _code;
	string _details;
	string _formatted;
};

class ErrorCode
{
	friend class Error;
public:
	ErrorCode(const string& message);

	explicit operator string() const;
	explicit operator const string&() const;
	explicit operator uint32_t() const;

private:
	uint32_t _code;
	string   _message;

	static uint32_t s_errorCodes;
};

template <typename Expected_t, typename Unexpected_t> 
using Result = tl::expected<Expected_t, Unexpected_t>;


#define FIRE_ERROR( ERROR_CODE, DETAILS ) \
	tl::make_unexpected( Error( ERROR_CODE, DETAILS ) )

#define FIRE_FORWARD_ERROR( ERR ) \
	tl::make_unexpected( ERR )
	

#define FIRE_RESULT( VALUE ) \
	Result< std::decay<decltype(VALUE)>::type, Error >(VALUE)

#define FIRE_RESULT_OK Result<void,ErrorCode>()


#define FIRE_ERRORCODE(SYMBOL) static const ErrorCode* SYMBOL

#define FIRE_ERRORCODE_DEF(SYMBOL, MESSAGE) \
	const ::Firestorm::ErrorCode* SYMBOL = new ErrorCode("["#SYMBOL"] "##MESSAGE)

CLOSE_NAMESPACE(Firestorm);
#endif