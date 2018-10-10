///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  IComponent
//
//  It is everything and it is nothing. It is, the Universe.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Project Universe 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBEXISTENCE_UNIVERSE_H_
#define LIBEXISTENCE_UNIVERSE_H_
#pragma once

#include "System.h"
#include "Entity.h"

OPEN_NAMESPACE(Firestorm);

/**
	\brief The base interface for a component.

	Now I know what you're going to say, "but Forgemistress, why are you using a virtual class whhen yoou have a
	data driven approach?" Well kick your shoes off and sit down son, because mama's got a little lesson for you.

	Use all the tools at your disposal.

	Virtual functions and interfaces have their place. It is when they're used and abused that the problems begin to
	arise. The IComponent interface provides functions to the engine systems that are only seldomly called,
	and so the performance impact is kept to a minimum. Plus, the functions are designed to act on groups of objects
	rather than on singular objects, so the number of calls to the virtual functions is kept to a minimum
	while the implementations do all of the work.
 **/
class Component
{
public:
private:
};

CLOSE_NAMESPACE(Firestorm);
#endif
