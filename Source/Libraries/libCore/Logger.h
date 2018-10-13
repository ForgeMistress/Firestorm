////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Logger
//
//  Outputs a log to the ostream passed to it.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_LOGGER_H_
#define LIBIO_LOGGER_H_
#pragma once

#include "libCore.h"
#include <iostream>
#include <mutex>

OPEN_NAMESPACE(Firestorm);

static string Format(const char* fmt, ...)
{
	string s;
	va_list l;
	va_start(l, fmt);
	s.append_sprintf_va_list(fmt, l);
	va_end(l);
	return s;
}

class Logger
{
public:
	Logger(std::ostream& ostream);

	void Write(const char* format, va_list list)
	{
		std::unique_lock lock(_s_allLock);
		string s;
		s.append_sprintf_va_list(format, list);
		_ostream << s.c_str()<<std::endl<<std::flush;
	}

	static Logger DEBUG_LOGGER;
	static Logger WARN_LOGGER;
	static Logger ERROR_LOGGER;

private:
	static std::mutex _s_allLock;
	std::ostream& _ostream;
};

static void FIRE_LOG_DEBUG(const char* format, ...)
{
	va_list l;
	va_start(l, format);
	Logger::DEBUG_LOGGER.Write(format, l);
	va_end(l);
}

static void FIRE_LOG_DEBUG(const char* format, va_list l)
{
	Logger::DEBUG_LOGGER.Write(format, l);
}

static void FIRE_LOG_WARNING(const char* format, ...)
{
	va_list l;
	va_start(l, format);
	Logger::WARN_LOGGER.Write(format, l);
	va_end(l);
}

static void FIRE_LOG_WARNING(const char* format, va_list l)
{
	Logger::WARN_LOGGER.Write(format, l);
}

static void FIRE_LOG_ERROR(const char* format, ...)
{
	va_list l;
	va_start(l, format);
	Logger::ERROR_LOGGER.Write(format, l);
	va_end(l);
}

static void FIRE_LOG_ERROR(const char* format, va_list l)
{
	Logger::ERROR_LOGGER.Write(format, l);
}

CLOSE_NAMESPACE(Firestorm);

#endif