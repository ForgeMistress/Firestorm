///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ArgParser
//
//  Parses command line arguments.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ArgParser.h"

#include "libCore.h"
#include "Logger.h"
#include <istream>
#include <sstream>

OPEN_NAMESPACE(Firestorm);

ArgParser::ArgParser(int ac, char** av)
{
	for(int i = 0; i < ac; ++i)
	{
		string arg(av[i]);
		vector<string> splitString(SplitString(av[i], '='));
		for(size_t j=0;j<splitString.size();++j)
		{
			FIRE_LOG_DEBUG("Arg %d: %s", j, splitString[j].c_str());
		}

		if(!splitString.empty())
		{
			if(splitString.size() == 1)
			{
				_args[splitString[0]] = "__FIRESTORM_TRUE_VALUE__";
			}
			else
			{
				_args[splitString[0]] = splitString[1];
			}
		}
	}
}

bool ArgParser::Get(const string& key, bool def)
{
	if(_args.find(key) == _args.end())
		return def;

	if(_args[key] == "__FIRESTORM_TRUE_VALUE__")
		return true;

	return def;
}

string ArgParser::Get(const string& key, const string& def)
{
	if(_args.find(key) == _args.end())
		return def;
	return _args[key];
}

string ArgParser::Get(const string& key, const char* def)
{
	return Get(key, string(def));
}

bool ArgParser::Has(const string& key) const
{
	return _args.find(key) != _args.end();
}

CLOSE_NAMESPACE(Firestorm);