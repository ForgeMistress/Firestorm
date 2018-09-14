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

OPEN_NAMESPACE(Firestorm);


class Logger
{
public:
	Logger(std::ostream& ostream);

	template <class T, class ...Args>
	void Write(T& firstArg, Args... args);

	static Logger DEBUG_LOGGER;
	static Logger WARN_LOGGER;
	static Logger ERROR_LOGGER;

private:
	template<class T, class... Args>
	void DoWrite(T& arg, Args... args);
	void DoWrite();

	std::ostream& m_ostream;
};


template <class T, class... Args>
void Logger::Write(T& firstArg, Args... args)
{
	m_ostream << firstArg;
	DoWrite(args...);
}


template <class T, class... Args>
void Logger::DoWrite(T& arg, Args... args)
{
	m_ostream << " " << arg;
	DoWrite(args...);
}

#define DEFINE_LOGGER_FUNCTION(LEVEL) \
template<class... Args>\
extern void FIRE_LOG_##LEVEL##(Args... args)\
{\
	Logger::##LEVEL##_LOGGER.Write(args...);\
}

template<class... Args>
extern void FIRE_LOG_DEBUG(Args... args)
{
	Logger::DEBUG_LOGGER.Write(args...);
}

template<class... Args>
extern void FIRE_LOG_WARNING(Args... args)
{
	Logger::WARNING_LOGGER.Write(args...);
}

template<class... Args>
extern void FIRE_LOG_ERROR(Args... args)
{
	Logger::ERROR_LOGGER.Write(args...);
}

CLOSE_NAMESPACE(Firestorm);

#endif