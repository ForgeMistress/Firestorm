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

#include "Window.h"
#include "InputEvents.h"

#include <libCore/libCore.h>
#include <libCore/ArgParser.h>

#include <libCore/Logger.h>

#include <libScene/RenderMgr.h>

#include <libCore/TaskGraph.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Application* Application::g_theApp = nullptr;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ApplicationWantsToCloseEvent::ApplicationWantsToCloseEvent(Application* app)
: App(app)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Application::Application(std::thread::id mainThreadId)
: _mainThreadId(mainThreadId)
, _managerMgr(*this)
{
	FIRE_ASSERT_MSG(g_theApp == nullptr, "only one instance of an Application can exist at a time");
	if(g_theApp == nullptr)
	{
		g_theApp = this;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
	_managerMgr.Shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::Initialize(int ac, char** av)
{
	_args = eastl::make_unique<ArgParser>(ac, av);

	_managerMgr.Initialize();

	OnInitialize(ac, av);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Application::Run()
{
	_mainThreadId = std::this_thread::get_id();

	bool windowWantsToClose{ false };
	double deltaT{ 0.0 };
	bool isRunning{ true };
	bool eventDispatched{ false };

	auto& renderMgr = _managerMgr.GetRenderMgr();
	Window& window = _managerMgr.GetWindow();
	TaskGraph& taskGraph = _managerMgr.GetTaskGraph();
	while(isRunning)
	{
		window.Process();
		// _mainThreadId = std::this_thread::get_id();

		/*timer->MeasureTime();
		auto end = std::chrono::system_clock::now();
		if(std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 0)
		{
			std::cout << "Rendertime: " << timer->GetDeltaTime() << ", FPS: " << 1.0 / timer->GetDeltaTime() << '\n';
			start = end;
		}*/

		OnUpdate(deltaT);
		OnRender();

		taskGraph.dispatch();

		//_surface->SwapBuffers();

		//windowWantsToClose = !_surface->PollEvents();

		/*bool hasCloseRegistered = Dispatcher.HasRegisteredEvents<ApplicationWantsToCloseEvent>();
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
		}*/
	}
	//_surface->Close();
	
	return OnShutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::Close()
{
	//_surface->SetWindowShouldClose(true);
	_closeAllowed = true;
	OnClose();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::AllowClose()
{
	FIRE_ASSERT(_waitingForCloseResponse == true && "Can not call AllowClose while not waiting for a close response");
	_waitingForCloseResponse = false;
	_closeAllowed = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::DisallowClose()
{
	FIRE_ASSERT(_waitingForCloseResponse == true && "Can not call AllowClose while not waiting for a close response");
	_waitingForCloseResponse = false;
	_closeAllowed = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vector2 Application::GetResolution() const
{
	//LLGL::Extent2D res(_surface->GetContentSize());
	//return { static_cast<float>(res.width), static_cast<float>(res.height) };
	return { 800,600 };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	GLOBAL SYSTEMS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const ArgParser& Application::Args() const
{
	return *_args.get();
}

ManagerMgr& Application::GetSystems() const
{
	return _managerMgr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	END GLOBAL SYSTEMS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void Application::OnChar(Surface* surface, unsigned int c)
//{
//	Dispatcher.Dispatch(CharEvent{ this, c });
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void Application::OnCharMods(Surface* surface, unsigned int codepoint, int mods)
//{
//	Dispatcher.Dispatch(CharModsEvent{ this, codepoint, mods });
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void Application::OnMouseButtonDown(Surface* surface, int mouseButton, int mods)
//{
//	Dispatcher.Dispatch(MouseButtonEvent{ this, mouseButton, true, mods });
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void Application::OnMouseButtonUp(Surface* surface, int mouseButton, int mods)
//{
//	Dispatcher.Dispatch(MouseButtonEvent{ this, mouseButton, false, mods });
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void Application::OnMouseMoved(Surface* surface, const Vector2& mousePos)
//{
//	_previousMousePos = _currentMousePos;
//	_currentMousePos = mousePos;
//	Dispatcher.Dispatch(MouseMoveEvent{ this, _previousMousePos,_currentMousePos });
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void Application::OnMouseScroll(Surface* surface, const Vector2& wheelOffset)
//{
//	Dispatcher.Dispatch(ScrollWheelEvent{ this, wheelOffset });
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void Application::OnKeyDown(Surface* surface, int key, int scancode, int mods)
//{
//	Dispatcher.Dispatch(KeyEvent{ this, key, scancode, mods, true });
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void Application::OnKeyUp(Surface* surface, int key, int scancode, int mods)
//{
//	Dispatcher.Dispatch(KeyEvent{ this, key, scancode, mods, false });
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void Application::OnKeyRepeat(Surface* surface, int key, int scancode, int mods)
//{
//	Dispatcher.Dispatch(KeyRepeatEvent{ this, key, scancode, mods });
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::DoSetJoystickUserData(void* userData, int jid)
{
	//_surface->SetJoystickUserData(jid, userData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* Application::DoGetJoystickUserData(int jid)
{
	//return _surface->GetJoystickUserData(jid);
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Application* Application::TheApp()
{
	return g_theApp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);