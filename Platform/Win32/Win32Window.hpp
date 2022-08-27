#pragma once

#include <functional>

#include <GLFW/glfw3.h>

#include "Core/Input/InputEvent.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

class Win32Window : public Window
{
public:
    Win32Window(WindowSpecification specification);
    ~Win32Window();

    virtual void OnUpdate() override;
    virtual void SetCallbackFunction(std::function<void(InputEvent &)> callback) override;
    virtual void SetVSync(bool enabled) override;
    virtual void *GetNativeWindow() override;

    virtual uint32_t GetWidth() override;
    virtual uint32_t GetHeight() override;

private:
    GLFWwindow *mWindowHandle;

    struct WindowCallback
    {
        std::function<void(InputEvent &)> CallbackFunction;
    };

    WindowCallback mCallback;
};

} // namespace Piksela