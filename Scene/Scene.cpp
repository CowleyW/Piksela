#include "Scene.hpp"
#include "Core/Core.hpp"
#include "Core/Input/InputEvent.hpp"
#include "Engines/Rendering/Renderer.hpp"
#include "Engines/Rendering/Shader.hpp"

namespace Piksela
{

Scene::Scene()
{
    Renderer::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    mCamera = std::make_shared<PerspectiveCamera>(glm::vec3(2.0f), 45.0f, 0.1f, 10.0f);
}

Scene::~Scene()
{
}

void Scene::Update()
{
}

void Scene::Render()
{
    Renderer::ClearBuffer();

    Renderer::BeginScene(mCamera);
    static int accumulator = 0;
    float pos = (glm::sin(++accumulator / 180.0f) * 2) + 3;
    mCamera->SetPosition(glm::vec3(pos));

    Renderer::EndScene();
}

void Scene::OnInputEvent(InputEvent &e)
{
}

} // namespace Piksela