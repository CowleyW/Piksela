#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include "Core/Core.hpp"
#include "DebugLayer.hpp"
#include "Platform/Window.hpp"

namespace Piksela
{

DebugLayer::DebugLayer(const std::shared_ptr<Window> &window)
{
    mWindow = window;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    io.Fonts->AddFontFromFileTTF("C:/dev/Piksela/Assets/Fonts/Roboto-Medium.ttf", 24.0f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)window->GetNativeWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

DebugLayer::~DebugLayer()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void DebugLayer::BeginDebugRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void DebugLayer::EndDebugRender()
{
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    io.DisplaySize = ImVec2(mWindow->GetWidth(), mWindow->GetHeight());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugLayer::Render(float timestep)
{
    ImGui::Begin("Debug");

    ImGui::Text("Frame time: %.1fms", timestep * 1000);
    ImGui::Text("FPS: %d", (int)(1 / timestep));

    ImGui::End();
}

} // namespace Piksela