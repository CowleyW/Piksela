#pragma once

#include "Core.hpp"
#include "Platform/Window.hpp"
#include "Scene/Scene.hpp"

namespace Piksela
{

struct ApplicationSpecification
{
    std::string Name;
    uint32_t WindowWidth;
    uint32_t WindowHeight;
    bool WindowDecorated;
    bool FullScreen;
    bool StartMaximized;
    bool VSync;
};

class Application
{
public:
    Application(ApplicationSpecification specification);
    ~Application();

    void Run();

private:
    void Application::OnEvent(InputEvent &e);

private:
    std::shared_ptr<Window> mWindow;

    std::unique_ptr<Scene> mScene;

    ApplicationSpecification mSpecification;
    bool mRunning;
};

Application *CreateEditorApplication(int argc, char **argv);
Application *CreateRuntimeApplication(int argc, char **argv);

} // namespace Piksela