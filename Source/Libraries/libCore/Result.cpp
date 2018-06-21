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

ResultCode Result::OK    = Result("");
ResultCode Result::ERROR = Result("Generic Error Result");

Result::Result(const String& message)
: m_message(message)
{
}

const String& Result::GetMessage() const
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

bool Result::operator==(ResultCode result) const
{
	// TODO: Temporary.
	return result.m_message == m_message;
}

CLOSE_NAMESPACE(Elf);