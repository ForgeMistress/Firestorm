///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  InputEvents
//
//  Contains the event structures for various events relating to user input.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "InputEvents.h"

OPEN_NAMESPACE(Firestorm);

FIRE_MIRROR_DEFINE(MouseButtonEvent)
{
}

MouseButtonEvent::MouseButtonEvent(Application* app, int button, bool isDown, int mods)
: App(app)
, Button(button)
, IsDown(isDown)
, Mods(mods)
{
}

FIRE_MIRROR_DEFINE(MouseMoveEvent)
{
}

MouseMoveEvent::MouseMoveEvent(Application* app, const Vector2& previousPos, const Vector2& currentPos)
: App(app)
, PreviousPos(previousPos)
, CurrentPos(currentPos)
{
}

Vector2 MouseMoveEvent::GetDelta() const
{
	return Vector2 {
		PreviousPos.x() - CurrentPos.x(),
		PreviousPos.y() - CurrentPos.y()
	};
}

ScrollWheelEvent::ScrollWheelEvent(Application* app, const Vector2& offset)
: App(app)
, Offset(offset)
{
}

FIRE_MIRROR_DEFINE(CharEvent)
{
}

CharEvent::CharEvent(Application* app, unsigned int c)
: App(app)
, Char(c)
{
}

FIRE_MIRROR_DEFINE(CharModsEvent)
{
}

CharModsEvent::CharModsEvent(Application* app, unsigned int c, int mods)
	: App(app)
	, Char(c)
	, Mods(mods)
{
}

FIRE_MIRROR_DEFINE(KeyEvent)
{
}

KeyEvent::KeyEvent(Application* app, int key, int scancode, int mods, bool isDown)
: App(app)
, Key(key)
, Scancode(scancode)
, Mods(mods)
, IsDown(isDown)
{
}

FIRE_MIRROR_DEFINE(KeyRepeatEvent)
{
}

KeyRepeatEvent::KeyRepeatEvent(Application* app, int key, int scancode, int mods)
: App(app)
, Key(key)
, Scancode(scancode)
, Mods(mods)
{
}

FIRE_MIRROR_DEFINE(JoystickConnectedEvent)
{
}

JoystickConnectedEvent::JoystickConnectedEvent(Application* app, int jid)
: App(app)
, JID(jid)
{
}

FIRE_MIRROR_DEFINE(JoystickDisconnectedEvent)
{
}

JoystickDisconnectedEvent::JoystickDisconnectedEvent(Application* app, int jid)
: App(app)
, JID(jid)
{
}

CLOSE_NAMESPACE(Firestorm);