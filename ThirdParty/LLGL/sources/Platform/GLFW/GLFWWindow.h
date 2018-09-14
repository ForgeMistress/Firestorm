/*
 * GLFWWindow.h
 *
 * This file is part of the "LLGL" project (Copyright (c) 2015-2018 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#ifndef LLGL_GLFW_WINDOW_H
#define LLGL_GLFW_WINDOW_H
#pragma once

#include <LLGL/Window.h>
#include <LLGL/Platform/NativeHandle.h>

struct GLFWwindow;

namespace LLGL
{

class GLFWWindow : public Window
{
public:
    GLFWWindow(const LLGL::WindowDescriptor& desc);
    virtual ~GLFWWindow();

    // LLGL implementations.
    void GetNativeHandle(void* nativeHandle) const override;

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

    void HandleFilesDropped(int numDropped, const char** filenames);
protected:
    virtual void OnProcessEvents();

    void InitializeWindowHandle(const LLGL::WindowDescriptor& desc);

private:
    mutable LLGL::WindowDescriptor _descriptor;
    GLFWwindow* _windowHandle{ nullptr };
};

}

#endif