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
#include "Application.h"

#include "InputEvents.h"

#include <libCore/libCore.h>

OPEN_NAMESPACE(Firestorm);

Application* Application::g_theApp = nullptr;

FIRE_MIRROR_DEFINE(ApplicationWantsToCloseEvent)
{
}

ApplicationWantsToCloseEvent::ApplicationWantsToCloseEvent(Application* app)
: App(app)
{

}

Application::Application()
{
	FIRE_ASSERT(g_theApp == nullptr && "only one instance of an Application can exist at a time");
	if(g_theApp == nullptr)
	{
		g_theApp = this;
	}
}

Application::~Application()
{
}

void Application::Initialize(int ac, char** av)
{
	_surface = new Surface(this, LLGL::Extent2D{ 640,480 }, "Firestorm Window");

	_surface->SetInputListener(this);
	_surface->SetJoystickCallback([](int jid, int event) -> void {
		if(event == GLFW_CONNECTED)
		{
			Application::TheApp()->Dispatcher.Dispatch(JoystickConnectedEvent{ Application::TheApp(), jid });
		}
		else if(event == GLFW_DISCONNECTED)
		{
			Application::TheApp()->Dispatcher.Dispatch(JoystickDisconnectedEvent{ Application::TheApp(), jid });
		}
	});
	OnInitialize(ac, av);
}

int Application::Run()
{
	bool windowWantsToClose{ false };
	double deltaT{ 0.0 };
	bool isRunning{ true };
	bool eventDispatched{ false };
	while(isRunning)
	{
		OnUpdate(deltaT);
		_surface->SwapBuffers();
		windowWantsToClose = !_surface->PollEvents();
		bool hasCloseRegistered = Dispatcher.HasRegisteredEvents<ApplicationWantsToCloseEvent>();

		if(windowWantsToClose)
		{
			if(hasCloseRegistered)
			{
				_waitingForCloseResponse = true;
				if(!eventDispatched)
				{
					Dispatcher.Dispatch(ApplicationWantsToCloseEvent(this));
				}
			}
			else
			{
				_closeAllowed = true;
				isRunning = false;
			}
		}
	}
	_surface->Close();

	return OnShutdown();
}

void Application::Close()
{
	_surface->SetWindowShouldClose(true);
	_closeAllowed = true;
	OnClose();
}

void Application::AllowClose()
{
	FIRE_ASSERT(_waitingForCloseResponse == true && "Can not call AllowClose while not waiting for a close response");
	_waitingForCloseResponse = false;
	_closeAllowed = true;
}

void Application::DisallowClose()
{
	FIRE_ASSERT(_waitingForCloseResponse == true && "Can not call AllowClose while not waiting for a close response");
	_waitingForCloseResponse = false;
	_closeAllowed = false;
}

void Application::OnChar(Surface* surface, unsigned int c)
{
	Dispatcher.Dispatch(CharEvent{ this, c });
}

void Application::OnCharMods(Surface* surface, unsigned int codepoint, int mods)
{
	Dispatcher.Dispatch(CharModsEvent{ this, codepoint, mods });
}

void Application::OnMouseButtonDown(Surface* surface, int mouseButton, int mods)
{
	Dispatcher.Dispatch(MouseButtonEvent{ this, mouseButton, true, mods });
}

void Application::OnMouseButtonUp(Surface* surface, int mouseButton, int mods)
{
	Dispatcher.Dispatch(MouseButtonEvent{ this, mouseButton, false, mods });
}

void Application::OnMouseMoved(Surface* surface, const Vector2& mousePos)
{
	_previousMousePos = _currentMousePos;
	_currentMousePos = mousePos;
	Dispatcher.Dispatch(MouseMoveEvent{ this, _previousMousePos,_currentMousePos });
}

void Application::OnMouseScroll(Surface* surface, const Vector2& wheelOffset)
{
	Dispatcher.Dispatch(ScrollWheelEvent{ this, wheelOffset });
}

void Application::OnKeyDown(Surface* surface, int key, int scancode, int mods)
{
	Dispatcher.Dispatch(KeyEvent{ this, key, scancode, mods, true });
}

void Application::OnKeyUp(Surface* surface, int key, int scancode, int mods)
{
	Dispatcher.Dispatch(KeyEvent{ this, key, scancode, mods, false });
}

void Application::OnKeyRepeat(Surface* surface, int key, int scancode, int mods)
{
	Dispatcher.Dispatch(KeyRepeatEvent{ this, key, scancode, mods });
}

void Application::DoSetJoystickUserData(void* userData, int jid)
{
	_surface->SetJoystickUserData(jid, userData);
}

void* Application::DoGetJoystickUserData(int jid)
{
	return _surface->GetJoystickUserData(jid);
}

Application* Application::TheApp()
{
	return g_theApp;
}

CLOSE_NAMESPACE(Firestorm);