#include "stdafx.h"
#include "libCore.h"

#include <sstream>

void Firestorm::libCore::Initialize(int ac, char** av)
{
}

Firestorm::Vector<Firestorm::String> Firestorm::SplitString(const String& str, char delim)
{
	Vector<String> tokens;
	String token;
	std::istringstream tokenStream(str);
	while(std::getline(tokenStream, token, delim))
	{
		tokens.push_back(token);
	}
	return tokens;
}