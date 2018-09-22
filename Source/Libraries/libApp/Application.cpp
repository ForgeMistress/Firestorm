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
#include <libCore/ArgParser.h>

#include <libCore/Logger.h>

#include <libScene/RenderMgr.h>

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Application* Application::g_theApp = nullptr;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FIRE_MIRROR_DEFINE(Firestorm::ApplicationWantsToCloseEvent)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ApplicationWantsToCloseEvent::ApplicationWantsToCloseEvent(Application* app)
: App(app)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Application::Application(Thread::id mainThreadId)
: _mainThreadId(mainThreadId)
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
	//_objectMaker.Shutdown();
	// _renderMgr.Shutdown();
	//_renderMgr.System->Release(*_renderMgr.Context);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::Initialize(int ac, char** av)
{
	_args = std::make_unique<ArgParser>(ac, av);

	auto& renderMgr = _managerMgr.GetRenderMgr();

	// Initialize the rendering context.
	LLGL::RenderContextDescriptor contextDesc;
	{
		contextDesc.videoMode.resolution = { 800,600 };
		contextDesc.vsync.enabled = true;
		contextDesc.profileOpenGL.contextProfile = LLGL::OpenGLContextProfile::CoreProfile;
	}
	renderMgr.Initialize("OpenGL", contextDesc);

	/*LLGL::Extent2D size(800, 600);
	_surface = new Surface(this, size, "Firestorm Window");

	_surface->SetInputListener(this);
	_surface->SetJoystickCallback([](int jid, int event) -> void {
		if (event == GLFW_CONNECTED)
		{
			Application::TheApp()->Dispatcher.Dispatch(JoystickConnectedEvent{ Application::TheApp(), jid });
		}
		else if (event == GLFW_DISCONNECTED)
		{
			Application::TheApp()->Dispatcher.Dispatch(JoystickDisconnectedEvent{ Application::TheApp(), jid });
		}
	});*/

	auto& window = static_cast<LLGL::Window&>(renderMgr.Context->GetSurface());
	window.SetTitle(L"Firestorm Application");
	window.Show();

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

	auto timer = LLGL::Timer::Create();
	auto start = std::chrono::system_clock::now();

	auto& renderMgr = _managerMgr.GetRenderMgr();

	while(static_cast<LLGL::Window&>(renderMgr.Context->GetSurface()).ProcessEvents())
	{
		_mainThreadId = std::this_thread::get_id();

		timer->MeasureTime();
		auto end = std::chrono::system_clock::now();
		if(std::chrono::duration_cast<std::chrono::seconds>(end - start).count() > 0)
		{
			std::cout << "Rendertime: " << timer->GetDeltaTime() << ", FPS: " << 1.0 / timer->GetDeltaTime() << '\n';
			start = end;
		}

		OnUpdate(deltaT);

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
		renderMgr.Context->Present();
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

void Application::EnableWindowResizing(bool enabled) const
{
	auto& renderMgr = _managerMgr.GetRenderMgr();
	LLGL::Window& window = static_cast<LLGL::Window&>(renderMgr.Context->GetSurface());
	LLGL::WindowDescriptor desc = window.GetDesc();
	desc.resizable = enabled;
	window.SetDesc(desc);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnChar(Surface* surface, unsigned int c)
{
	Dispatcher.Dispatch(CharEvent{ this, c });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnCharMods(Surface* surface, unsigned int codepoint, int mods)
{
	Dispatcher.Dispatch(CharModsEvent{ this, codepoint, mods });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnMouseButtonDown(Surface* surface, int mouseButton, int mods)
{
	Dispatcher.Dispatch(MouseButtonEvent{ this, mouseButton, true, mods });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnMouseButtonUp(Surface* surface, int mouseButton, int mods)
{
	Dispatcher.Dispatch(MouseButtonEvent{ this, mouseButton, false, mods });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnMouseMoved(Surface* surface, const Vector2& mousePos)
{
	_previousMousePos = _currentMousePos;
	_currentMousePos = mousePos;
	Dispatcher.Dispatch(MouseMoveEvent{ this, _previousMousePos,_currentMousePos });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnMouseScroll(Surface* surface, const Vector2& wheelOffset)
{
	Dispatcher.Dispatch(ScrollWheelEvent{ this, wheelOffset });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnKeyDown(Surface* surface, int key, int scancode, int mods)
{
	Dispatcher.Dispatch(KeyEvent{ this, key, scancode, mods, true });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnKeyUp(Surface* surface, int key, int scancode, int mods)
{
	Dispatcher.Dispatch(KeyEvent{ this, key, scancode, mods, false });
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Application::OnKeyRepeat(Surface* surface, int key, int scancode, int mods)
{
	Dispatcher.Dispatch(KeyRepeatEvent{ this, key, scancode, mods });
}

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