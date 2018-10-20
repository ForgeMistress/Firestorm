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
#include "Window.h"

#include "ManagerMgr.h"

#include <libMirror/ObjectMaker.h>
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


class Application
{
public:
	Application(std::thread::id mainThreadID);
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

	class ManagerMgr& GetSystems() const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	END GLOBAL SYSTEMS
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void OnInitialize(int ac, char** av) = 0;
	virtual void OnUpdate(double deltaT) = 0;
	virtual void OnRender() = 0;
	virtual void OnClose() = 0;
	virtual int  OnShutdown() = 0;

public:
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

	std::thread::id _mainThreadId;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	GLOBAL SYSTEMS
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	eastl::unique_ptr<ArgParser> _args;
	mutable ManagerMgr           _managerMgr;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	END GLOBAL SYSTEMS
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

CLOSE_NAMESPACE(Firestorm);

// InitializeLib<::Firestorm::libJson>(ac, av);

#define FIRE_RUN_APPLICATION(CLASS_NAME)                                                \
	int main(int ac, char** av)                                     			        \
	{                                                                                   \
		::Firestorm::InitializeLib<::Firestorm::libApp>(ac,av);           			    \
		::Firestorm::InitializeLib<::Firestorm::libCore>(ac,av);           			    \
		::Firestorm::InitializeLib<::Firestorm::libExistence>(ac,av);           		\
		::Firestorm::InitializeLib<::Firestorm::libIO>(ac,av);           			    \
		::Firestorm::InitializeLib<::Firestorm::libMath>(ac,av);           			    \
		::Firestorm::InitializeLib<::Firestorm::libMirror>(ac,av);           			\
		::Firestorm::InitializeLib<::Firestorm::libScene>(ac,av);           			\
		::Firestorm::InitializeLib<::Firestorm::libScript>(ac,av);           			\
		::Firestorm::InitializeLib<::Firestorm::libSerial>(ac,av);           			\
		::Firestorm::InitializeLib<::Firestorm::libUI>(ac,av);           			    \
                                                                                        \
        int result = 0;                                                                 \
		::Firestorm::Application* app = nullptr;                                        \
																						\
		app = new CLASS_NAME(std::this_thread::get_id());                               \
		FIRE_ASSERT(app && "application could not be initialized"); 			        \
		app->Initialize(ac, av);                                    			        \
		result = app->Run();                                    			            \
		                                                                                \
		if(app)                                                                         \
			delete app;                                                 			    \
		libCore::ReportMemoryLeaks();                                                   \
		return result;                                              			        \
	}

#endif