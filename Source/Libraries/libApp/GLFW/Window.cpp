////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Window
//
//  Abstracts the creation of a window.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Window.h"

OPEN_NAMESPACE(Firestorm);

Window::Window(const LLGL::WindowDescriptor& desc)
: _descriptor(desc)
, _windowHandle(nullptr)
{
}

Window::~Window()
{
}

// LLGL implementations.
void Window::SetPosition(const LLGL::Offset2D& position)
{

}

LLGL::Offset2D Window::GetPosition() const
{

}

void Window::SetSize(const LLGL::Extent2D& size, bool useClientArea)
{

}

LLGL::Extent2D Window::GetSize(bool useClientArea) const
{

}

void Window::SetTitle(const std::wstring& title)
{

}

std::wstring Window::GetTitle() const
{
	return std::wstring();
}

void Window::Show(bool show)
{
	_isShowing = show;
	if (show)
		glfwShowWindow(_windowHandle);
	else
		glfwHideWindow(_windowHandle);
}

bool Window::IsShown() const
{
	return _isShowing;
}

namespace {
	void GLFW_DropFunc(GLFWwindow* windowHandle, int numDropped, const char** filenames)
	{
		Firestorm::Window* win = (Firestorm::Window*)glfwGetWindowUserPointer(windowHandle);
		win->HandleFilesDropped(numDropped, filenames);
	}
}
/*
std::wstring    title;
Offset2D        position;
Extent2D        size;
bool            visible             = false;
bool            borderless          = false;
bool            resizable = false;
bool            acceptDropFiles = false;
bool            preventForPowerSafe = false;
bool            centered = false;
const void*     windowContext = nullptr;
*/

void Window::SetDesc(const LLGL::WindowDescriptor& desc)
{
	_descriptor = desc;

	if(_windowHandle)
	{
		glfwSetWindowTitle(_windowHandle, (char*)desc.title.c_str());
		glfwSetWindowPos(_windowHandle, desc.position.x, desc.position.y);
		glfwSetWindowSize(_windowHandle, desc.size.width, desc.size.height);

		if(!desc.visible) { glfwHideWindow(_windowHandle); }
		else { glfwShowWindow(_windowHandle); }

		glfwSetWindowAttrib(_windowHandle, GLFW_DECORATED, desc.borderless ? GLFW_FALSE : GLFW_TRUE);
		glfwSetWindowAttrib(_windowHandle, GLFW_RESIZABLE, desc.resizable ? GLFW_TRUE : GLFW_FALSE);
		if(desc.acceptDropFiles) { glfwSetDropCallback(_windowHandle, GLFW_DropFunc); }
		else { glfwSetDropCallback(_windowHandle, nullptr); }
		// centered needs implementation
		// windowContext needs to be dealt with
	}

}

LLGL::WindowDescriptor Window::GetDesc() const
{
}

void Window::HandleFilesDropped(int numDropped, const char** filenames)
{

}

void Window::OnProcessEvents()
{

}

CLOSE_NAMESPACE(Firestorm);