///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Application
//
//  The base application class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBCORE_ARGPARSER_H_
#define LIBCORE_ARGPARSER_H_
#pragma once

OPEN_NAMESPACE(Firestorm);

class ArgParser final
{
public:
	ArgParser(int ac, char** av);
	
	String Get(const String& key, const String& def="");
	String Get(const String& key, const char* def = "");
	bool Get(const String& key, bool def = false);
	bool Has(const String& key) const;

private:
	Vector<String> Split(const String& s, char delimeter) const;

	UnorderedMap<String, String> _args;
};

CLOSE_NAMESPACE(Firestorm);

#endif
