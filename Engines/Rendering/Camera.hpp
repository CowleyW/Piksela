#pragma once
#include <glm/glm.hpp>

namespace Piksela
{

class PerspectiveCamera
{
public:
    PerspectiveCamera(const glm::vec3 &position, float fov, float nearClip, float farClip);

    glm::mat4 GetViewProjectionMatrix();

    void SetPosition(const glm::vec3 &position);

private:
    void RecalculateViewMatrix();
    void RecalculateProjectionMatrix();

private:
    glm::mat4 mViewProjection;
    glm::mat4 mView;
    glm::mat4 mProjection;
    glm::vec3 mPosition;

    float mFOV;
    float mNearClip;
    float mFarClip;
};

} // namespace Piksela