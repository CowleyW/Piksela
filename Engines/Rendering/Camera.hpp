#pragma once
#include <glm/glm.hpp>

namespace Piksela
{

class PerspectiveCamera
{
public:
    PerspectiveCamera(const glm::vec3 &position, float fov, float nearClip, float farClip);

    glm::mat4 GetViewProjectionMatrix() const;

    void Translate(const glm::vec3 &delta);
    void Rotate(const glm::vec3 &delta);
    void SetPosition(const glm::vec3 &position);

private:
    void RecalculateViewMatrix();
    void RecalculateProjectionMatrix();

private:
    glm::mat4 mViewProjection;
    glm::mat4 mView;
    glm::mat4 mProjection;
    glm::vec3 mPosition;
    glm::vec3 mFront;

    float mFOV;
    float mNearClip;
    float mFarClip;
};

} // namespace Piksela