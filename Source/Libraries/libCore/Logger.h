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


class Logger
{
public:
	Logger(std::ostream& ostream);

	template <class T, class ...Args>
	void Write(T& firstArg, Args... args);

	inline std::ostream& mprintf(const char * fstr) throw()
	{
		return m_ostream << fstr;
	}

	template<typename T, typename... Args>
	std::ostream& mprintf(const char * fstr, const T & x) throw()
	{
		size_t i = 0;
		char c = fstr[0];

		while(c != '%')
		{
			if (c == 0) return m_ostream; // string is finished
			m_ostream << c;
			c = fstr[++i];
		};
		c = fstr[++i];
		m_ostream << x;

		if (c == 0) return m_ostream; // 

		// print the rest of the stirng
		m_ostream << &fstr[++i];
		return m_ostream;
	}


	template<typename T, typename... Args>
	std::ostream & mprintf(const char * fstr, const T & x, Args... args) throw()
	{
		size_t i = 0;
		char c = fstr[0];

		while(c != '%')
		{
			if (c == 0) return m_ostream; // string is finished
			m_ostream << c;
			c = fstr[++i];
		}
		c = fstr[++i];
		m_ostream << x;

		if (c == 0) return m_ostream; // string is finished

		return mprintf(&fstr[++i], args...);
	}


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

template<class... Args>
static void FIRE_LOG_DEBUG(const char* format, Args... args)
{
	// static std::mutex lock;
	// std::scoped_lock<std::mutex> l(lock);
	String f = String(format) + "\n";
	Logger::DEBUG_LOGGER.mprintf(f.c_str(), args...);
}

template<class... Args>
static void FIRE_LOG_WARNING(const char* format, Args... args)
{
	// static std::mutex lock;
	// std::scoped_lock<std::mutex> l(lock);
	String f = String(format) + "\n";
	Logger::WARNING_LOGGER.mprintf(f.c_str(), args...);
}

template<class... Args>
static void FIRE_LOG_ERROR(const char* format, Args... args)
{
	// static std::mutex lock;
	// std::scoped_lock<std::mutex> l(lock);
	String f = String(format) + "\n";
	Logger::ERROR_LOGGER.mprintf(f.c_str(), args...);
}

CLOSE_NAMESPACE(Firestorm);

#endif