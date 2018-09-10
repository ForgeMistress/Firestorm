///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Application
//
//  The base application class.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBAPP_APPLICATION_H_
#define LIBAPP_APPLICATION_H_
#pragma once

#include "Surface.h"

#include <libMirror/EventDispatcher.h>

OPEN_NAMESPACE(Firestorm);

struct ApplicationWantsToCloseEvent
{
	FIRE_MIRROR_DECLARE(ApplicationWantsToCloseEvent);
public:
	ApplicationWantsToCloseEvent(class Application* app);

	class Application* App;
};

class Application : public IInputEventListener
{
public:
	Application();
	virtual ~Application();

	void Initialize(int ac, char** av);
	int  Run();
	void Close();

	void AllowClose();
	void DisallowClose();

	virtual void OnInitialize(int ac, char** av) = 0;
	virtual void OnUpdate(double deltaT) = 0;
	virtual void OnClose() = 0;
	virtual int  OnShutdown() = 0;

public:
	// IInputEventListener
	virtual void OnChar(Surface* surface, unsigned int c);
	virtual void OnCharMods(Surface* surface, unsigned int codepoint, int mods);
	virtual void OnMouseButtonDown(Surface* surface, int mouseButton, int mods);
	virtual void OnMouseButtonUp(Surface* surface, int mouseButton, int mods);
	virtual void OnMouseMoved(Surface* surface, const Vector_2D& mousePos);
	virtual void OnMouseScroll(Surface* surface, const Vector_2D& wheelOffset);
	virtual void OnKeyDown(Surface* surface, int key, int scancode, int mods);
	virtual void OnKeyUp(Surface* surface, int key, int scancode, int mods);
	virtual void OnKeyRepeat(Surface* surface, int key, int scancode, int mods);

	template <class T>
	void SetJoystickUserData(T* userData, int jid)
	{
		DoSetJoystickUserData((void*)userData, jid);
	}

	template<class T>
	T* GetJoystickUserData(int jid)
	{
		return static_cast<T*>(DoGetJoystickUserData(jid));
	}

	static Application* TheApp();

public:
	EventDispatcher Dispatcher;

private:
	void DoSetJoystickUserData(void* userData, int jid);
	void* DoGetJoystickUserData(int jid);

	static Application* g_theApp;

	bool _waitingForCloseResponse{ false };
	bool _closeAllowed{ false };

	Vector_2D _previousMousePos;
	Vector_2D _currentMousePos;

	RefPtr<Surface> _surface{ nullptr };
};

CLOSE_NAMESPACE(Firestorm);

#define FIRE_RUN_APPLICATION(CLASS_NAME) \
	int main(int ac, char** av) \
	{ \
		::Firestorm :: libApp :: Initialize(ac,av);\
		::Firestorm :: libCore :: Initialize(ac,av);\
		::Firestorm :: libExistence :: Initialize(ac,av);\
		::Firestorm :: libIO :: Initialize(ac,av);\
		::Firestorm :: libJson :: Initialize(ac,av);\
		::Firestorm :: libMath :: Initialize(ac,av);\
		::Firestorm :: libMirror :: Initialize(ac,av); \
		::Firestorm :: libScene :: Initialize(ac,av);\
		::Firestorm :: libScript :: Initialize(ac,av);\
		::Firestorm :: libSerial :: Initialize(ac,av);\
		::Firestorm :: libUI :: Initialize(ac,av);\
\
		::Firestorm::Application* app = new CLASS_NAME(); \
		FIRE_ASSERT(app && "application could not be initialized"); \
		app->Initialize(ac, av); \
		int result = app->Run(); \
		delete app; \
		return result; \
	}

#endif