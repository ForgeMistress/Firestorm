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
	static Logger WARNING_LOGGER;
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

CLOSE_NAMESPACE(Firestorm);

#define FIRE_LOG(OUTPUT, ...) ::Firestorm::Logger::##OUTPUT##_LOGGER.Write(__VA_ARGS__)

/**
	Log to the default std::cout ostream.
 **/
#define FIRE_LOG_DEBUG(...) FIRE_LOG(DEBUG, __VA_ARGS__)

/**
	Log to the default std::cout ostream.
 **/
#define FIRE_LOG_WARNING(...) FIRE_LOG(WARNING, __VA_ARGS__)

/**
	Log to the default std::cerr ostream.
 **/
#define FIRE_LOG_ERROR(...) FIRE_LOG(ERROR, __VA_ARGS__)

#endif