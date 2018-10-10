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
#include "ObjectMaker.h"

#include "ManagerMgr.h"
#include <libMirror/EventDispatcher.h>
#include <libCore/ArgParser.h>

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
	Application(Thread::id mainThreadID);
	virtual ~Application();

	void Initialize(int ac, char** av);
	int  Run();
	void Close();

	void AllowClose();
	void DisallowClose();

	Vector2 GetResolution() const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	GLOBAL SYSTEMS
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/**
		Retrieve the ArgParser for this Application.
	 **/
	const ArgParser& Args() const;

	ManagerMgr& GetSystems() const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	END GLOBAL SYSTEMS
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EnableWindowResizing(bool enabled) const;

	virtual void OnInitialize(int ac, char** av) = 0;
	virtual void OnUpdate(double deltaT) = 0;
	virtual void OnRender() = 0;
	virtual void OnClose() = 0;
	virtual int  OnShutdown() = 0;

public:
	// IInputEventListener
	virtual void OnChar(Surface* surface, unsigned int c);
	virtual void OnCharMods(Surface* surface, unsigned int codepoint, int mods);
	virtual void OnMouseButtonDown(Surface* surface, int mouseButton, int mods);
	virtual void OnMouseButtonUp(Surface* surface, int mouseButton, int mods);
	virtual void OnMouseMoved(Surface* surface, const Vector2& mousePos);
	virtual void OnMouseScroll(Surface* surface, const Vector2& wheelOffset);
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

	Vector2 _previousMousePos;
	Vector2 _currentMousePos;

	Surface* _surface{ nullptr };

	Thread::id _mainThreadId;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	GLOBAL SYSTEMS
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::unique_ptr<ArgParser> _args;
	mutable ManagerMgr         _managerMgr;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	END GLOBAL SYSTEMS
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

CLOSE_NAMESPACE(Firestorm);

// InitializeLib<::Firestorm::libJson>(ac, av);

#define FIRE_RUN_APPLICATION(CLASS_NAME)                                           \
	template <class T>                                                             \
	static void InitializeLib(int ac, char** av)                                   \
	{																			   \
		try																		   \
		{																		   \
			Library<T>::Initialize(ac, av);									       \
		}																		   \
		catch(std::exception& e)												   \
		{																		   \
			FIRE_LOG_ERROR("Exception initializing lib :: Error -> %s", e.what()); \
			std::cout<<std::flush;                                                 \
			throw std::runtime_error("lib init exception encountered");			   \
		}																		   \
	}																			   \
	int main(int ac, char** av)                                     			   \
	{                                                                              \
		InitializeLib<::Firestorm::libApp>(ac,av);           			           \
		InitializeLib<::Firestorm::libCore>(ac,av);           			           \
		InitializeLib<::Firestorm::libExistence>(ac,av);           			       \
		InitializeLib<::Firestorm::libIO>(ac,av);           			           \
		InitializeLib<::Firestorm::libMath>(ac,av);           			           \
		InitializeLib<::Firestorm::libMirror>(ac,av);           			       \
		InitializeLib<::Firestorm::libScene>(ac,av);           			           \
		InitializeLib<::Firestorm::libScript>(ac,av);           			       \
		InitializeLib<::Firestorm::libSerial>(ac,av);           			       \
		InitializeLib<::Firestorm::libUI>(ac,av);           			           \
                                                                    			   \
		::Firestorm::Application* app = new CLASS_NAME(std::this_thread::get_id());\
		FIRE_ASSERT(app && "application could not be initialized"); 			   \
		app->Initialize(ac, av);                                    			   \
		int result = app->Run();                                    			   \
		delete app;                                                 			   \
		libCore::ReportMemoryLeaks();                                              \
		return result;                                              			   \
	}

#endif