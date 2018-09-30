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

#include <iostream>
#include <mutex>

OPEN_NAMESPACE(Firestorm);

static std::ostream& _Format(std::ostream& os, const char * fstr) throw()
{
	return os << fstr;
}

template<typename T, typename... Args>
static std::ostream& _Format(std::ostream& os, const char * fstr, const T & x) throw()
{
	size_t i = 0;
	char c = fstr[0];

	while(c != '%')
	{
		if (c == 0) return os; // string is finished
		os << c;
		c = fstr[++i];
	};
	c = fstr[++i];
	os << x;

	if(c == 0) return os; // 

								  // print the rest of the stirng
	os << &fstr[++i];
	return os;
}


template<typename T, typename... Args>
static std::ostream& _Format(std::ostream& os, const char * fstr, const T & x, Args... args) throw()
{
	size_t i = 0;
	char c = fstr[0];

	while(c != '%')
	{
		if (c == 0) return os; // string is finished
		os << c;
		c = fstr[++i];
	}
	c = fstr[++i];
	os << x;

	if(c == 0) return os; // string is finished

	return _Format(os, &fstr[++i], args...);
}

template<class... Args>
extern String Format(const char* fmt, Args... args)
{
	std::ostringstream oss;
	_Format(oss, fmt, std::forward<Args>(args)...);
	return oss.str();
}

class Logger
{
public:
	Logger(std::ostream& ostream);

	template<class... Args>
	void Write(const char* format, Args... args)
	{
		std::unique_lock<std::mutex> lock(_s_allLock);
		_Format(_ostream, format, std::forward<Args>(args)...);
		_ostream << std::endl << std::flush;
	}

	static Logger DEBUG_LOGGER;
	static Logger WARN_LOGGER;
	static Logger ERROR_LOGGER;

private:
	static std::mutex _s_allLock;
	std::ostream& _ostream;
};

template<class... Args>
static void FIRE_LOG_DEBUG(const char* format, Args... args)
{
	Logger::DEBUG_LOGGER.Write(format, args...);
}

template<class... Args>
static void FIRE_LOG_WARNING(const char* format, Args... args)
{
	Logger::WARNING_LOGGER.Write(format, args...);
}

template<class... Args>
static void FIRE_LOG_ERROR(const char* format, Args... args)
{
	Logger::ERROR_LOGGER.Write(format, args...);
}

CLOSE_NAMESPACE(Firestorm);

#endif