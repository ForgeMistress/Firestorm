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

#include <libIO/Logger.h>

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
	//_renderMgr.System->Release(*_renderMgr.Context);
}

void Application::Initialize(int ac, char** av)
{
	_args = new ArgParser(ac, av);

	try
	{
		// Initialize the rendering context.
		_renderMgr.System = LLGL::RenderSystem::Load("OpenGL");

		LLGL::RenderContextDescriptor contextDesc;
		{
			contextDesc.videoMode.resolution = { 800,600 };
			contextDesc.vsync.enabled = true;
			contextDesc.profileOpenGL.contextProfile = LLGL::OpenGLContextProfile::CoreProfile;
		}
		_renderMgr.Context = _renderMgr.System->CreateRenderContext(contextDesc);

		// Print renderer information
		const auto& info = _renderMgr.System->GetRendererInfo();

		std::cout << "Renderer:         " << info.rendererName << std::endl;
		std::cout << "Device:           " << info.deviceName << std::endl;
		std::cout << "Vendor:           " << info.vendorName << std::endl;
		std::cout << "Shading Language: " << info.shadingLanguageName << std::endl;

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

		auto& window = static_cast<LLGL::Window&>(_renderMgr.Context->GetSurface());
		window.SetTitle(L"Firestorm Application");
		window.Show();

		OnInitialize(ac, av);
	}
	catch(std::exception& e)
	{
		FIRE_LOG_ERROR("Error Loading RenderSystem: ", e.what());
	}
}

int Application::Run()
{
	bool windowWantsToClose{ false };
	double deltaT{ 0.0 };
	bool isRunning{ true };
	bool eventDispatched{ false };

	auto timer = LLGL::Timer::Create();
	auto start = std::chrono::system_clock::now();

	while(static_cast<LLGL::Window&>(_renderMgr.Context->GetSurface()).ProcessEvents())
	{
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
		_renderMgr.Context->Present();
	}
	//_surface->Close();
	
	return OnShutdown();
}

void Application::Close()
{
	//_surface->SetWindowShouldClose(true);
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

const ArgParser& Application::Args() const
{
	return *_args;
}

Vector2 Application::GetResolution() const
{
	//LLGL::Extent2D res(_surface->GetContentSize());
	//return { static_cast<float>(res.width), static_cast<float>(res.height) };
	return { 800,600 };
}

RenderMgr& Application::GetRenderMgr()
{
	return _renderMgr;
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
	//_surface->SetJoystickUserData(jid, userData);
}

void* Application::DoGetJoystickUserData(int jid)
{
	//return _surface->GetJoystickUserData(jid);
	return nullptr;
}

Application* Application::TheApp()
{
	return g_theApp;
}

CLOSE_NAMESPACE(Firestorm);