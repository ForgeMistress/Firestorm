////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Surface
//
//  Implementation of LLGL::Surface using GLFW.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBAPP_SURFACE_H_
#define LIBAPP_SURFACE_H_

#ifdef FIRE_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <LLGL/LLGL.h>
#include <LLGL/Surface.h>

#include <libMath/Vector.h>

OPEN_NAMESPACE(Firestorm);

class Surface;

class IInputEventListener
{
public:
	virtual void OnChar(Surface* surface, unsigned int c) = 0;
	virtual void OnCharMods(Surface* surface, unsigned int codepoint, int mods) = 0;
	virtual void OnMouseButtonDown(Surface* surface, int mouseButton, int mods) = 0;
	virtual void OnMouseButtonUp(Surface* surface, int mouseButton, int mods) = 0;
	virtual void OnMouseMoved(Surface* surface, const Vector_2D& mousePos) = 0;
	virtual void OnMouseScroll(Surface* surface, const Vector_2D& wheelOffset) = 0;
	virtual void OnKeyDown(Surface* surface, int key, int scancode, int mods) = 0;
	virtual void OnKeyUp(Surface* surface, int key, int scancode, int mods) = 0;
	virtual void OnKeyRepeat(Surface* surface, int key, int scancode, int mods) = 0;
};

class IJoystickEventListener
{
public:
	virtual void OnJoystickConnected(Surface* surface, int jid) = 0;
	virtual void OnJoystickDisconnected(Surface* surface, int jid) = 0;
};

class Surface : public LLGL::Surface
{
public:
	Surface(class Application* app, const LLGL::Extent2D& size, const String& title);
	virtual ~Surface();

	void GetNativeHandle(void* nativeHandle) const override;
	LLGL::Extent2D GetContentSize() const override;
	bool AdaptForVideoMode(LLGL::VideoModeDescriptor& videoModeDesc) override;
	void ResetPixelFormat() override;

	bool PollEvents();
	void SetWindowShouldClose(bool value);
	void Close();

	void SetInputListener(IInputEventListener* listener);
	IInputEventListener* GetInputListener() const;
	void SetJoystickCallback(void(*Callback)(int, int));

	void* GetJoystickUserData(int jid) const;
	void SetJoystickUserData(int jid, void* userData);

private:
	GLFWwindow* CreateGLFWWindow();

	class Application* _app{ nullptr };
	String _title;
	LLGL::Extent2D _size;
	GLFWwindow* _window{ nullptr };
	IInputEventListener* _listener{ nullptr };
};

CLOSE_NAMESPACE(Firestorm);

#endif
