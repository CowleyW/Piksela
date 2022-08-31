#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Camera.hpp"
#include "Renderer.hpp"

#include <iostream>

namespace Piksela
{

PerspectiveCamera::PerspectiveCamera(const glm::vec3 &position, float fov, float nearClip, float farClip) :
        mPosition(position), mFOV(fov), mNearClip(nearClip), mFarClip(farClip)
{
    mView = glm::lookAt(mPosition, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mProjection = glm::perspective(glm::radians(mFOV), Renderer::GetFramebufferAspectRatio(), mNearClip, mFarClip);
    mViewProjection = mProjection * mView;
}

glm::mat4 PerspectiveCamera::GetViewProjectionMatrix()
{
    return mViewProjection;
}

void PerspectiveCamera::SetPosition(const glm::vec3 &position)
{
    mPosition = position;
    RecalculateViewMatrix();
}

void PerspectiveCamera::RecalculateViewMatrix()
{
    mView = glm::lookAt(mPosition, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    mViewProjection = mProjection * mView;
}

void PerspectiveCamera::RecalculateProjectionMatrix()
{
    mProjection = glm::perspective(glm::radians(mFOV), Renderer::GetFramebufferAspectRatio(), mNearClip, mFarClip);
    mViewProjection = mProjection * mView;
}

} // namespace Piksela