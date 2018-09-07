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

#ifdef ELF_PLATFORM_WINDOWS
#include "../Window.h"
#include <GLFW/include/GLFW/glfw3.h>
#include <thread>

OPEN_NAMESPACE(Elf);

static void GLFW_PumpEmptyEvent();

struct Window::Impl::ImplData
{
	static uint8_t numWindows;
	static volatile bool unlockThreadRunning;
	static volatile bool killUnlockThread;
	static std::thread* unlockThread;

	ImplData()
	{
	}

	~ImplData()
	{
	}

	void WindowIsShown()
	{
		numWindows++;
		if(!unlockThread)
		{
			unlockThread = new std::thread(GLFW_PumpEmptyEvent);
		}
	}

	void WindowIsHidden()
	{
		numWindows--;
		if(numWindows == 0)
		{
			unlockThreadRunning = false;
		}
	}

	GLFWwindow* window{ nullptr };
	String title;
};

std::thread* Window::Impl::ImplData::unlockThread = nullptr;
uint8_t Window::Impl::ImplData::numWindows = 0;
volatile bool Window::Impl::ImplData::unlockThreadRunning = true;
volatile bool Window::Impl::ImplData::killUnlockThread = false;


void GLFW_PumpEmptyEvent()
{
	static std::chrono::microseconds duration(1);
	while(!Window::Impl::ImplData::killUnlockThread)
	{
		if(Window::Impl::ImplData::unlockThreadRunning)
		{
			std::this_thread::sleep_for(duration);
			glfwPostEmptyEvent();
		}
	}
}

Window::Impl::Impl(Window* window)
: window(window)
, data(new ImplData)
{
}

Window::Impl::~Impl()
{
}

bool Window::Impl::Initialize(const String& title, Vector_2D size)
{
	if(!glfwInit())
	{
		return false;
	}
	data->title = title;
	data->window = glfwCreateWindow(size.x, size.y, data->title.c_str(), nullptr, nullptr);

	data->WindowIsShown();

	return true;
}

const String& Window::Impl::GetTitle() const
{
	return data->title;
}

void Window::Impl::SetTitle(const String& title)
{
	data->title = title;
}

Vector_2D Window::Impl::GetDimensions() const
{
	assert(data->window);
	int x, y;
	glfwGetWindowSize(data->window, &x, &y);
	return Vector_2D(x, y);
}

void Window::Impl::SetDimensions(const Vector_2D& size)
{
	assert(data->window);
	glfwSetWindowSize(data->window, size.x, size.y);
}

void Window::Impl::Update()
{
	glfwWaitEvents();
	if(glfwWindowShouldClose(data->window))
	{
		// TODO: Add some kind of interception possibilities here.
		Close();
	}
}

void Window::Impl::Close()
{
	data->WindowIsHidden();
}

CLOSE_NAMESPACE(Elf);

#endif