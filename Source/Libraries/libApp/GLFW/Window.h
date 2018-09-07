////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Window
//
//  Abstracts the creation of a window that provides a graphics context to draw to.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBAPP_WINDOW_H_
#define LIBAPP_WINDOW_H_
#pragma once

#include <libMath/Vector.h>
#include <LLGL/Window.h>
#include <GLFW/glfw3.h>

OPEN_NAMESPACE(Firestorm);

class Window final : public LLGL::Window
{
public:
	Window(const LLGL::WindowDescriptor& desc);
	virtual ~Window();

	// LLGL implementations.
	virtual void SetPosition(const LLGL::Offset2D& position);
	virtual LLGL::Offset2D GetPosition() const;

	virtual void SetSize(const LLGL::Extent2D& size, bool useClientArea = true);
	virtual LLGL::Extent2D GetSize(bool useClientArea = true) const;

	virtual void SetTitle(const std::wstring& title);
	virtual std::wstring GetTitle() const;

	virtual void Show(bool show = true);
	virtual bool IsShown() const;

	virtual void SetDesc(const LLGL::WindowDescriptor& desc);
	virtual LLGL::WindowDescriptor GetDesc() const;

protected:
	virtual void OnProcessEvents();

private:
	bool _isVisible{ true };
	LLGL::WindowDescriptor _descriptor;
	GLFWwindow* _windowHandle;
};

CLOSE_NAMESPACE(Firestorm);

#endif