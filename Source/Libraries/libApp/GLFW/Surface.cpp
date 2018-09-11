////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Surface
//
//  Implementation of LLGL::Surface using GLFW.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Surface.h"
#include "../Application.h"

#include <libCore/libCore.h>
#include <glfw/glfw3native.h>
#include <LLGL/LLGL.h>
#include <LLGL/Platform/NativeHandle.h>

OPEN_NAMESPACE(Firestorm);

namespace {
	static void GetItems(GLFWwindow* window, Surface*& surface, IInputEventListener*& listener)
	{
		surface = nullptr;
		listener = nullptr;
		surface = (Surface*)glfwGetWindowUserPointer(window);
		FIRE_ASSERT(surface != nullptr && "glfw window has no surface associated with it");
		listener = surface->GetInputListener();
	}

	static void GLFW_CharCallback(GLFWwindow* window, unsigned int c)
	{
		Surface* surface;
		IInputEventListener* listener;
		GetItems(window, surface, listener);

		if (listener)
		{
			listener->OnChar(surface, c);
		}
	}

	static void GLFW_CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
	{
		Surface* surface;
		IInputEventListener* listener;
		GetItems(window, surface, listener);
		if (listener)
		{
			listener->OnCharMods(surface, codepoint, mods);
		}
	}

	static void GLFW_MouseButtonCallback(GLFWwindow* window, int mouseButton, int action, int mods)
	{
		Surface* surface;
		IInputEventListener* listener;
		GetItems(window, surface, listener);
		if (listener)
		{
			if (action == GLFW_PRESS)
			{
				listener->OnMouseButtonDown(surface, mouseButton, mods);
			}
			else if (action == GLFW_RELEASE)
			{
				listener->OnMouseButtonUp(surface, mouseButton, mods);
			}
		}
	}

	static void GLFW_CursorPosCallback(GLFWwindow* window, double mouseX, double mouseY)
	{
		Surface* surface;
		IInputEventListener* listener;
		GetItems(window, surface, listener);
		if (listener)
		{
			listener->OnMouseMoved(surface, Vector2{ (float)mouseX, (float)mouseY });
		}
	}

	static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Surface* surface;
		IInputEventListener* listener;
		GetItems(window, surface, listener);
		if (listener)
		{
			if (action == GLFW_PRESS)
			{
				listener->OnKeyDown(surface, key, scancode, mods);
			}
			else if (action == GLFW_RELEASE)
			{
				listener->OnKeyUp(surface, key, scancode, mods);
			}
			else if (action == GLFW_REPEAT)
			{
				listener->OnKeyRepeat(surface, key, scancode, mods);
			}
		}
	}

	static void GLFW_ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		Surface* surface;
		IInputEventListener* listener;
		GetItems(window, surface, listener);
		if (listener)
		{
			listener->OnMouseScroll(surface, Vector2{ (float)xOffset, (float)yOffset });
		}
	}

	/*static bool s_runThread = true;

	static void GLFW_Thread()
	{
		while(s_runThread)
		{

		}
	}*/
}

Surface::Surface(class Application* app, const LLGL::Extent2D& size, const String& title)
: _app(app)
, _size(size)
, _title(title)
, _window(CreateGLFWWindow())
//, _pumpBlankEventThread(GLFW_Thread)
{
}

Surface::~Surface()
{
	if(_window)
	{
		glfwDestroyWindow(_window);

		// TODO: Temporary! Must abstract this.
		glfwTerminate();
	}
}

void Surface::GetNativeHandle(void* nativeHandle) const
{
	auto handle = reinterpret_cast<LLGL::NativeHandle*>(nativeHandle);
	handle->window = glfwGetWin32Window(_window);
}

LLGL::Extent2D Surface::GetContentSize() const
{
	return _size;
}

bool Surface::AdaptForVideoMode(LLGL::VideoModeDescriptor& videoModeDesc)
{
	FIRE_ASSERT(_window != nullptr && "window was not created in Surface::AdaptForVideoMode");
	_size = videoModeDesc.resolution;
	glfwSetWindowSize(_window, _size.width, _size.height);
	return true;
}

void Surface::ResetPixelFormat()
{
	FIRE_ASSERT(_window != nullptr);
	glfwDestroyWindow(_window);
	_window = CreateGLFWWindow();
}

void Surface::SwapBuffers()
{
	glfwSwapBuffers(_window);
}

bool Surface::PollEvents()
{
	glfwPollEvents();
	return !glfwWindowShouldClose(_window);
}

void Surface::SetWindowShouldClose(bool value)
{
	glfwSetWindowShouldClose(_window, value ? GLFW_TRUE : GLFW_FALSE);
}

void Surface::Close()
{
	glfwDestroyWindow(_window);
	_window = nullptr;
}

void Surface::SetInputListener(IInputEventListener* listener)
{
	FIRE_ASSERT(_listener == nullptr && "can not set listener. it has already been set.");
	_listener = listener;
}

IInputEventListener* Surface::GetInputListener() const
{
	return _listener;
}

void Surface::SetJoystickCallback(void (*callback)(int, int))
{
	glfwSetJoystickCallback(callback);
}

void* Surface::GetJoystickUserData(int jid) const
{
	return glfwGetJoystickUserPointer(jid);
}

void Surface::SetJoystickUserData(int jid, void* userData)
{
	glfwSetJoystickUserPointer(jid, userData);
}

GLFWwindow* Surface::CreateGLFWWindow()
{
	if(glfwInit() == GLFW_FALSE)
	{
		throw std::runtime_error("could not initialize GLFW");
	}
	auto wnd = glfwCreateWindow(_size.width, _size.height, _title.c_str(), nullptr, nullptr);
	if(!wnd)
		throw std::runtime_error(String(String("could not create GLFW window titled ") + _title).c_str());

	glfwSetWindowUserPointer(wnd, this);
	glfwSetCharCallback(wnd, GLFW_CharCallback);
	glfwSetCharModsCallback(wnd, GLFW_CharModsCallback);
	glfwSetMouseButtonCallback(wnd, GLFW_MouseButtonCallback);
	glfwSetCursorPosCallback(wnd, GLFW_CursorPosCallback);
	glfwSetKeyCallback(wnd, GLFW_KeyCallback);
	glfwSetScrollCallback(wnd, GLFW_ScrollCallback);
	glfwMakeContextCurrent(wnd);
	glfwSwapInterval(1);

	return wnd;
}

CLOSE_NAMESPACE(Firestorm);