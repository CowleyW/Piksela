#pragma once

namespace Piksela
{

enum class EventType
{
    None = 0,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMinimize,
    WindowMaximize,
    WindowMove,
    KeyPressed,
    KeyReleased,
    KeyTyped,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
};

class InputEvent
{
public:
    virtual ~InputEvent() = default;

    virtual EventType GetType() const = 0;

public:
    bool Handled = false;
};

//===---------------------------------------------------------------------===//
// Window Events
//===---------------------------------------------------------------------===//

class WindowCloseEvent : public InputEvent
{
public:
    WindowCloseEvent() = default;

    virtual EventType GetType() const override
    {
        return EventType::WindowClose;
    }
};

class WindowResizeEvent : public InputEvent
{
public:
    WindowResizeEvent(uint32_t width, uint32_t height) :
            mWidth(width), mHeight(height)
    {
    }

    virtual EventType GetType() const override
    {
        return EventType::WindowResize;
    }

    uint32_t GetWidth()
    {
        return mWidth;
    }

    uint32_t GetHeight()
    {
        return mHeight;
    }

private:
    uint32_t mWidth;
    uint32_t mHeight;
};

//===---------------------------------------------------------------------===//
// Mouse Events
//===---------------------------------------------------------------------===//

class MouseMovedEvent : public InputEvent
{
public:
    MouseMovedEvent(double x, double y) :
            mX((float)x), mY((float)y)
    {
    }

    virtual EventType GetType() const override
    {
        return EventType::MouseMoved;
    }

    float GetX()
    {
        return mX;
    }

    float GetY()
    {
        return mY;
    }

private:
    float mX;
    float mY;
};

} // namespace Piksela