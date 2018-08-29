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

OPEN_NAMESPACE(Elf);

Window::Window()
: _impl(new Impl(this))
{
}

Window::~Window()
{
}

bool Window::Initialize(const String& title, Vector_2D size)
{
	return _impl->Initialize(title, size);
}

const String& Window::GetTitle() const
{
	return _impl->GetTitle();
}

void Window::SetTitle(const String& title)
{
	_impl->SetTitle(title);
}

Vector_2D Window::GetDimensions() const
{
	return _impl->GetDimensions();
}

void Window::SetDimensions(const Vector_2D& vec)
{
	_impl->SetDimensions(vec);
}

void Window::Update()
{
	_impl->Update();
}

void Window::Close()
{
	_impl->Close();
}

CLOSE_NAMESPACE(Elf);