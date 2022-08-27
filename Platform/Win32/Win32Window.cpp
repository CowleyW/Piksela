#include <functional>

#include <GLFW/glfw3.h>

#include "Core/Core.hpp"
#include "Core/Input/InputEvent.hpp"
#include "Win32Window.hpp"

namespace Piksela
{

Win32Window::Win32Window(WindowSpecification specification) :
        Window(specification)
{
    PK_ASSERT(glfwInit(), "Failed to initialize glfw!");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindowHandle = glfwCreateWindow(
            specification.WindowWidth,
            specification.WindowHeight,
            specification.Name.c_str(),
            nullptr,
            nullptr);

    PK_ASSERT(mWindowHandle, "Failed to create Window!");

    glfwSetWindowUserPointer(mWindowHandle, &mCallback);

    glfwSetWindowCloseCallback(mWindowHandle, [](GLFWwindow *windowHandle) {
        WindowCloseEvent e;

        auto &callback = *reinterpret_cast<WindowCallback *>(glfwGetWindowUserPointer(windowHandle));

        callback.CallbackFunction(e);
    });

    glfwSetFramebufferSizeCallback(mWindowHandle, [](GLFWwindow *windowHandle, int width, int height) {
        WindowResizeEvent e(width, height);

        auto &callback = *reinterpret_cast<WindowCallback *>(glfwGetWindowUserPointer(windowHandle));

        callback.CallbackFunction(e);
    });
}

Win32Window::~Win32Window()
{
    glfwDestroyWindow(mWindowHandle);

    glfwTerminate();
}

void Win32Window::OnUpdate()
{
    glfwPollEvents();
}

void Win32Window::SetCallbackFunction(std::function<void(InputEvent &)> callback)
{
    mCallback.CallbackFunction = callback;
}

void Win32Window::SetVSync(bool enabled)
{
    mSpecification.VSync = enabled;
}

void *Win32Window::GetNativeWindow()
{
    return mWindowHandle;
}

uint32_t Win32Window::GetWidth()
{
    return mSpecification.WindowWidth;
}

uint32_t Win32Window::GetHeight()
{
    return mSpecification.WindowHeight;
}

} // namespace Piksela