#include "Scene.hpp"
#include "Core/Core.hpp"
#include "Core/Input/InputEvent.hpp"
#include "Engines/Rendering/Renderer.hpp"

namespace Piksela
{

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Update()
{
}

void Scene::Render()
{
    Renderer::DrawFrame();
}

void Scene::OnInputEvent(InputEvent &e)
{
}

} // namespace Piksela