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
#include <GLFW/glfw3.h>
#include <libMirror/EventDispatcher.h>
#include "../ManagerMgr.h"
OPEN_NAMESPACE(Firestorm);

class Window final
{
public:
	Window(class Application& app, ManagerMgr& managerMgr);
	virtual ~Window();

	void Initialize(const struct WindowDesc& windowDesc);

	void Process();

	// LLGL implementations.
	void SetPosition(const Vector2& position);
	Vector2 GetPosition() const;

	void SetSize(const Vector2& size, bool useClientArea = true);
	Vector2 GetSize(bool useClientArea = true) const;

	void SetTitle(const char* title);
	const char* GetTitle() const;

	void SetVisible(bool show = true);
	bool IsVisible() const;

	Application& GetApp() const { return _app; }

	void HandleFilesDropped(int numDropped, const char** filenames);
protected:
	virtual void OnProcessEvents();

private:
	class Application& _app;
	ManagerMgr& _managerMgr;
	GLFWwindow* _windowHandle{ nullptr };
	string _title;
	bool _visible;
};

CLOSE_NAMESPACE(Firestorm);

#endif