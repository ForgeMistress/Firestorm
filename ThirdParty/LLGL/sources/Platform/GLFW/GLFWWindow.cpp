#include "GLFWWindow.h"

#include "../../Core/Helper.h"

#include <glfw/include/GLFW/glfw3.h>
#include <glfw/include/GLFW/glfw3native.h>
#include <assert.h>

namespace LLGL
{

GLFWWindow::GLFWWindow(const LLGL::WindowDescriptor& desc)
: _descriptor(desc)
{
    InitializeWindowHandle(desc);
}

GLFWWindow::~GLFWWindow()
{
}

void GLFWWindow::GetNativeHandle(void* nativeHandle) const
{

}

// LLGL implementations.
void GLFWWindow::SetPosition(const LLGL::Offset2D& position)
{
    if (_windowHandle)
        glfwSetWindowPos(_windowHandle, position.x, position.y);
}

LLGL::Offset2D GLFWWindow::GetPosition() const
{
    LLGL::Offset2D offset;
    if(_windowHandle)
        glfwGetWindowPos(_windowHandle, &offset.x, &offset.y);
    return offset;
}

void GLFWWindow::SetSize(const LLGL::Extent2D& size, bool useClientArea)
{
    if (_windowHandle)
        glfwSetWindowSize(_windowHandle, size.width, size.height);
}

LLGL::Extent2D GLFWWindow::GetSize(bool useClientArea) const
{
    int width, height;
    if (_windowHandle)
        glfwGetWindowSize(_windowHandle, &width, &height);
    return LLGL::Extent2D(width, height);
}

void GLFWWindow::SetTitle(const std::wstring& title)
{
}

std::wstring GLFWWindow::GetTitle() const
{
    return std::wstring();
}

void GLFWWindow::Show(bool show)
{
    assert(_windowHandle != nullptr && "window handle was not initialized in Window::Show()");
    _descriptor.visible = show;
    if (show)
    {
        glfwMakeContextCurrent(_windowHandle);
        glfwShowWindow(_windowHandle);
    }
    else
        glfwHideWindow(_windowHandle);
}

bool GLFWWindow::IsShown() const
{
    return _descriptor.visible;
}

namespace {
    void GLFW_DropFunc(GLFWwindow* windowHandle, int numDropped, const char** filenames)
    {
        GLFWWindow* win = (GLFWWindow*)glfwGetWindowUserPointer(windowHandle);
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

void GLFWWindow::SetDesc(const LLGL::WindowDescriptor& desc)
{
    assert(_windowHandle != nullptr && "window handle was not initialized in Window::GetDesc()");

    _descriptor = desc;

    glfwSetWindowTitle(_windowHandle, (char*)_descriptor.title.c_str());
    glfwSetWindowPos(_windowHandle, _descriptor.position.x, _descriptor.position.y);
    glfwSetWindowSize(_windowHandle, _descriptor.size.width, _descriptor.size.height);

    if (!_descriptor.visible) { glfwHideWindow(_windowHandle); }
    else { glfwShowWindow(_windowHandle); }

    glfwSetWindowAttrib(_windowHandle, GLFW_DECORATED, _descriptor.borderless ? GLFW_FALSE : GLFW_TRUE);
    glfwSetWindowAttrib(_windowHandle, GLFW_RESIZABLE, _descriptor.resizable ? GLFW_TRUE : GLFW_FALSE);
    if (_descriptor.acceptDropFiles) { glfwSetDropCallback(_windowHandle, GLFW_DropFunc); }
    else { glfwSetDropCallback(_windowHandle, nullptr); }
    // centered needs implementation
    // windowContext needs to be dealt with
}

LLGL::WindowDescriptor GLFWWindow::GetDesc() const
{
    assert(_windowHandle != nullptr && "window handle was not initialized in Window::GetDesc()");
    LLGL::WindowDescriptor desc;
    glfwGetWindowPos(_windowHandle, &desc.position.x, &desc.position.y);

    int w, h;
    glfwGetWindowSize(_windowHandle, &w, &h);
    _descriptor.size.width = (uint32_t)w;
    _descriptor.size.height = (uint32_t)h;

    _descriptor.borderless = glfwGetWindowAttrib(_windowHandle, GLFW_DECORATED) == GLFW_FALSE ? true : false;
    _descriptor.resizable = glfwGetWindowAttrib(_windowHandle, GLFW_RESIZABLE) == GLFW_TRUE ? true : false;
    return _descriptor;
}

void GLFWWindow::HandleFilesDropped(int numDropped, const char** filenames)
{
}

void GLFWWindow::OnProcessEvents()
{

}

void GLFWWindow::InitializeWindowHandle(const LLGL::WindowDescriptor& desc)
{
    if (glfwInit() == GLFW_TRUE)
    {
        _windowHandle = glfwCreateWindow(
            desc.size.width,
            desc.size.height,
            (const char*)desc.title.c_str(),
            nullptr,
            nullptr
        );
        if (!_windowHandle)
        {
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(_windowHandle);
    }
}

std::unique_ptr<Window> Window::Create(const WindowDescriptor& desc)
{
    return MakeUnique<GLFWWindow>(desc);
}

}