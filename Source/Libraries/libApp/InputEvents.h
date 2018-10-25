///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  InputEvents
//
//  Contains the event structures for various events relating to user input.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBAPP_INPUTEVENTS_H_
#define LIBAPP_INPUTEVENTS_H_
#pragma once

#include <libMath/Vector.h>
#include <libCore/Object.h>

OPEN_NAMESPACE(Firestorm);

class Application;

struct MouseButtonEvent
{
	FIRE_MIRROR_DECLARE(MouseButtonEvent);
public:
	MouseButtonEvent(Application* app, int button, bool isDown, int mods);
	
	Application* App;
	int Button;
	bool IsDown;
	int Mods;
};

struct MouseMoveEvent
{
	FIRE_MIRROR_DECLARE(MouseMoveEvent);
public:
	MouseMoveEvent(Application* app, const Vector2& pos);

	Application* App;
	Vector2 Pos;
};

struct ScrollWheelEvent
{
	FIRE_MIRROR_DECLARE(ScrollWheelEvent);
public:
	ScrollWheelEvent(Application* app, const Vector2& offset);

	Application* App;
	Vector2 Offset;
};

struct CharEvent
{
	FIRE_MIRROR_DECLARE(CharEvent);
public:
	CharEvent(Application* app, unsigned int c);

	Application* App;
	unsigned int Char;
};

struct CharModsEvent
{
	FIRE_MIRROR_DECLARE(CharModsEvent);
public:
	CharModsEvent(Application* app, unsigned int c, int mods);

	Application* App;
	unsigned int Char;
	int Mods;
};

struct KeyEvent
{
	FIRE_MIRROR_DECLARE(KeyEvent);
public:
	KeyEvent(Application* app, int key, int scancode, int mods, bool isDown);

	Application* App;
	int Key;
	int Scancode;
	int Mods;
	bool IsDown;
};

struct KeyRepeatEvent
{
	FIRE_MIRROR_DECLARE(KeyRepeatEvent);
public:
	KeyRepeatEvent(Application* app, int key, int scancode, int mods);

	Application* App;
	int Key;
	int Scancode;
	int Mods;
	bool IsDown;
};

struct JoystickConnectedEvent
{
	FIRE_MIRROR_DECLARE(JoystickConnectedEvent);
public:
	JoystickConnectedEvent(Application* app, int jid);

	Application* App;
	int JID;
};

struct JoystickDisconnectedEvent
{
	FIRE_MIRROR_DECLARE(JoystickDisconnectedEvent);
public:
	JoystickDisconnectedEvent(Application* app, int jid);

	Application* App;
	int JID;
};

CLOSE_NAMESPACE(Firestorm);

#endif