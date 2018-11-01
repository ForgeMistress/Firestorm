////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Window
//
//  Abstracts the creation of a window.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Window.h"
#include "../Window.h"
#include "../Application.h"
#include "../InputEvents.h"

OPEN_NAMESPACE(Firestorm);

namespace{
// glfw input events
static void GetItems(GLFWwindow* glfwWindow, Application*& app)
{
	app = (Application*)glfwGetWindowUserPointer(glfwWindow);
	FIRE_ASSERT_MSG(app != nullptr, "glfw window has no Firestorm Window associated with it");
}

static void GLFW_CharCallback(GLFWwindow* gWindow, unsigned int c)
{
	Application* app;
	GetItems(gWindow, app);

	if(app)
	{
		app->Dispatcher.Dispatch(CharEvent{ app, c });
	}
}

static void GLFW_CharModsCallback(GLFWwindow* window, unsigned int codepoint, int mods)
{
	Application* app;
	GetItems(window, app);

	if(app)
	{
		app->Dispatcher.Dispatch(CharModsEvent{ app, codepoint, mods });
	}
}

static void GLFW_MouseButtonCallback(GLFWwindow* window, int mouseButton, int action, int mods)
{
	Application* app;
	GetItems(window, app);

	if(app)
	{
		if(action == GLFW_PRESS)
		{
			app->Dispatcher.Dispatch(MouseButtonEvent{ app, mouseButton, true, mods });
		}
		else if(action == GLFW_RELEASE)
		{
			app->Dispatcher.Dispatch(MouseButtonEvent{ app, mouseButton, false, mods });
		}
	}
}

static void GLFW_CursorPosCallback(GLFWwindow* window, double mouseX, double mouseY)
{
	Application* app;
	GetItems(window, app);

	if(app)
	{
		app->Dispatcher.Dispatch(MouseMoveEvent{ app, Vector2((float)mouseX,(float)mouseY) });
	}
}

static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* app;
	GetItems(window, app);

	if(app)
	{
		if(action == GLFW_PRESS)
		{
			app->Dispatcher.Dispatch(KeyEvent{ app, key,scancode,mods,true });
		}
		else if(action == GLFW_RELEASE)
		{
			app->Dispatcher.Dispatch(KeyEvent{ app, key,scancode,mods,false });
		}
		else if(action == GLFW_REPEAT)
		{
			app->Dispatcher.Dispatch(KeyRepeatEvent{ app, key,scancode,mods });
		}
	}
}

static void GLFW_ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	Application* app;
	GetItems(window, app);

	if(app)
	{
		app->Dispatcher.Dispatch(ScrollWheelEvent{ 
			app,
			Vector2{
				narrow_cast<float>(xOffset),
				narrow_cast<float>(yOffset)
			}
		});
	}
}

}

Window::Window(Application& app)
: _app(app)
{
}

Window::~Window()
{
	if(_windowHandle)
	{
		glfwDestroyWindow(_windowHandle);
	}
}

void Window::Initialize(const WindowDesc& desc)
{
	if(glfwInit() == GLFW_TRUE)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		_windowHandle = glfwCreateWindow(
			desc.Width,
			desc.Height,
			desc.Title,
			nullptr,
			nullptr
		);
		if(!_windowHandle)
		{
			glfwTerminate();
			return;
		}
		glfwSetWindowUserPointer(_windowHandle, &_app);
	}
}

void Window::Process()
{
	glfwPollEvents();
}

void Window::SetPosition(const Vector2& position)
{
	if(_windowHandle)
		glfwSetWindowPos(_windowHandle, narrow<int>(position.x), narrow<int>(position.y));
}

Vector2 Window::GetPosition() const
{
	int w, h;
	if(_windowHandle)
		glfwGetWindowPos(_windowHandle, &w, &h);
	return Vector2(narrow<float>(w), narrow<float>(h));
}

void Window::SetSize(const Vector2& size, bool useClientArea)
{
	if(_windowHandle)
		glfwSetWindowSize(_windowHandle, narrow<int>(size.x), narrow<int>(size.y));
}

Vector2 Window::GetSize(bool useClientArea) const
{
	int width, height;
	if(_windowHandle)
		glfwGetWindowSize(_windowHandle, &width, &height);
	return Vector2(narrow<float>(width), narrow<float>(height));
}

void Window::SetTitle(const char* title)
{
	_title = title;
}

const char* Window::GetTitle() const
{
	return _title.c_str();
}

void Window::SetVisible(bool visible)
{
	FIRE_ASSERT(_windowHandle != nullptr && "window handle was not initialized in Window::Show()");
	_visible = visible;
	if(visible)
	{
		glfwMakeContextCurrent(_windowHandle);
		glfwShowWindow(_windowHandle);
	}
	else
		glfwHideWindow(_windowHandle);
}

bool Window::IsVisible() const
{
	return _visible;
}

namespace {
	void GLFW_DropFunc(GLFWwindow* windowHandle, int numDropped, const char** filenames)
	{
		Firestorm::Window* win = (Firestorm::Window*)glfwGetWindowUserPointer(windowHandle);
		win->HandleFilesDropped(numDropped, filenames);
	}
}

void Window::HandleFilesDropped(int numDropped, const char** filenames)
{
}

void Window::OnProcessEvents()
{

}

CLOSE_NAMESPACE(Firestorm);