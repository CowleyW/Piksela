#include "Application.hpp"
#include "Core.hpp"
#include "Engines/Rendering/Renderer.hpp"
#include "Input/Input.hpp"
#include "Platform/Window.hpp"
#include "Scene/Scene.hpp"
#include "Time.hpp"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

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

    Time::Init(0.01f);
    Input::Init(mWindow);
    Renderer::Init(mWindow);

    mScene = std::make_unique<Scene>();

    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)mWindow->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
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
            Renderer::ResizeViewport(ev.GetWidth(), ev.GetHeight());
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
    float accumulator = 0.0f;

    bool show_demo_window = true;

    while (mRunning)
    {
        float timestep = Time::GetTimestep();
        float fixedTimestep = Time::GetFixedTimestep();
        accumulator += timestep;

        // Update
        while (accumulator >= fixedTimestep)
        {
            mScene->Update(fixedTimestep);

            accumulator -= fixedTimestep;
        }

        // TODO: Interpolate

        // Render
        mScene->Render();

        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        io.DisplaySize = ImVec2(mWindow->GetWidth(), mWindow->GetHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // End frame
        mWindow->Update();
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