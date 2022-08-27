#include "Application.hpp"
#include "Core.hpp"
#include "Engines/Rendering/Renderer.hpp"
#include "Platform/Window.hpp"
#include "Scene/Scene.hpp"

namespace Piksela
{

Application::Application(ApplicationSpecification specification) :
        mSpecification(specification), mRunning(true)
{
    WindowSpecification windowSpec = {};
    windowSpec.Name = specification.Name;
    windowSpec.WindowWidth = specification.WindowWidth;
    windowSpec.WindowHeight = specification.WindowHeight;
    windowSpec.WindowDecorated = specification.WindowDecorated;
    windowSpec.FullScreen = specification.FullScreen;
    windowSpec.VSync = specification.VSync;

    mWindow = Window::InitWindow(windowSpec);
    mWindow->SetCallbackFunction([this](InputEvent &e) {
        OnEvent(e);
    });

    Renderer::Init(mWindow);

    mScene = std::make_unique<Scene>();
}

Application::~Application()
{
    Renderer::Shutdown();
}

void Application::OnEvent(InputEvent &e)
{
    switch (e.GetType())
    {
        case EventType::WindowClose:
        {
            mRunning = false;
            e.Handled = true;
            return;
        }
        case EventType::WindowResize:
        {
            WindowResizeEvent &ev = static_cast<WindowResizeEvent &>(e);
            mWindow->SetWidth(ev.GetWidth());
            mWindow->SetHeight(ev.GetHeight());
            Renderer::ResizeViewport();
            ev.Handled = true;
            return;
        }
    }

    if (!e.Handled)
    {
        mScene->OnInputEvent(e);
    }
}

void Application::Run()
{
    while (mRunning)
    {
        mScene->Update();

        mScene->Render();

        mWindow->OnUpdate();
    }
}

Application *CreateEditorApplication(int argc, char **argv)
{
    CoreLogger::Init();

    ApplicationSpecification specification = {};
    specification.Name = "Piksela";
    specification.WindowWidth = 1920;
    specification.WindowHeight = 1080;
    specification.WindowDecorated = true;
    specification.FullScreen = false;
    specification.StartMaximized = false;
    specification.VSync = true;

    return new Application(specification);
}

Application *CreateRuntimeApplication(int argc, char **argv)
{
    CoreLogger::Init();
    PK_ASSERT(false, "Cannot create runtime application! Use editor application instead.");
    return nullptr;
}

} // namespace Piksela