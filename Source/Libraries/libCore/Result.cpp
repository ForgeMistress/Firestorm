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

OPEN_NAMESPACE(Elf);

const Result& Result::OK    = Result::MSG("");
const Result& Result::ERROR = Result::MSG("Generic Error Result");

// static
Result Result::MSG(const string& message)
{
	return Result(message);
}

Result::Result(const string& message)
: m_message(message)
{
}

const string& Result::GetMessage() const
{
	return m_message;
}

bool Result::IsOK() const
{
	return m_message.empty() == true;
}

bool Result::IsNotOK() const
{
	return m_message.empty() == false;
}

bool Result::operator==(const Result& result)
{
	// TODO: Temporary.
	return result.m_message == m_message;
}

CLOSE_NAMESPACE(Elf);