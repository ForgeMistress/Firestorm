///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Application
//
//  The base application class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ArgParser.h"
#include <istream>
#include <sstream>

OPEN_NAMESPACE(Firestorm);

ArgParser::ArgParser(int ac, char** av)
{
	std::cout << std::endl;
	for(int i = 0; i < ac; ++i)
	{
		String arg(av[i]);
		Vector<String> splitString(Split(av[i], '='));
		if(splitString.size() == 1)
		{
			_args[splitString[0]] = "__FIRESTORM_TRUE_VALUE__";
		}
		else
		{
			_args[splitString[0]] = splitString[1];
		}
		std::cout << "key=" << splitString[0] << " val=" << _args[splitString[0]] << std::endl << std::flush;
	}
}

bool ArgParser::Get(const String& key, bool def)
{
	if(_args.find(key) == _args.end())
		return def;

	if(_args[key] == "__FIRESTORM_TRUE_VALUE__")
		return true;

	return def;
}

String ArgParser::Get(const String& key, const String& def)
{
	if(_args.find(key) == _args.end())
		return def;
	return _args[key];
}

String ArgParser::Get(const String& key, const char* def)
{
	return Get(key, String(def));
}

bool ArgParser::Has(const String& key) const
{
	return _args.find(key) != _args.end();
}

Vector<String> ArgParser::Split(const String& s, char delimeter) const
{
	Vector<String> tokens;
	String token;
	std::istringstream tokenStream(s);
	while(std::getline(tokenStream, token, delimeter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

CLOSE_NAMESPACE(Firestorm);