#pragma once

#include "Core/Core.hpp"
#include "Core/Input/InputEvent.hpp"

namespace Piksela
{

struct WindowSpecification
{
    std::string Name;
    uint32_t WindowWidth;
    uint32_t WindowHeight;
    bool WindowDecorated;
    bool FullScreen;
    bool VSync;
};

class Window
{
public:
    ~Window();

    virtual void OnUpdate() = 0;
    virtual void SetCallbackFunction(std::function<void(InputEvent &)> callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual void *GetNativeWindow() = 0;

    virtual uint32_t GetWidth() = 0;
    virtual uint32_t GetHeight() = 0;

    void SetWidth(uint32_t width);
    void SetHeight(uint32_t height);

    static std::shared_ptr<Window> InitWindow(WindowSpecification specification);

protected:
    Window(WindowSpecification specification);

protected:
    WindowSpecification mSpecification;
};

} // namespace Piksela