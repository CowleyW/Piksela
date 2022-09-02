#include <GLFW/glfw3.h>

#include "Win32Input.hpp"

namespace Piksela
{

std::shared_ptr<Window> Win32Input::sWindow;

void Win32Input::Init(std::shared_ptr<Window> window)
{
    sWindow = window;
}

bool Win32Input::IsKeyPressed(KeyCode keycode)
{
    GLFWwindow *window = (GLFWwindow *)sWindow->GetNativeWindow();
    int state = glfwGetKey(window, static_cast<int>(keycode));

    return (state == GLFW_PRESS);
}

bool Win32Input::IsMouseButtonPressed(MouseButton button)
{
    GLFWwindow *window = (GLFWwindow *)sWindow->GetNativeWindow();
    int state = glfwGetMouseButton(window, static_cast<int>(button));

    return (state == GLFW_PRESS);
}

} // namespace Piksela