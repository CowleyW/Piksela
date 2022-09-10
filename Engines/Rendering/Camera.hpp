#pragma once
#include <glm/glm.hpp>

namespace Piksela
{

class PerspectiveCamera
{
public:
    PerspectiveCamera(const glm::vec3 &position, float fov, float nearClip, float farClip);

    glm::mat4 GetViewProjectionMatrix() const;
    glm::vec3 GetPosition() const;
    glm::vec3 GetFront() const
    {
        return mFront;
    }

    void Translate(const glm::vec3 &delta);
    void Rotate(const glm::vec3 &delta);
    void SetPosition(const glm::vec3 &position);

    /// @brief Called by the Scene when the window resizes.
    void RecalculateProjectionMatrix();

private:
    void RecalculateViewMatrix();

private:
    glm::mat4 mViewProjection;
    glm::mat4 mView;
    glm::mat4 mProjection;
    glm::vec3 mPosition;
    glm::vec3 mFront;

    // [pitch, yaw, roll]
    glm::vec3 mRotation;
    // glm::quat mOrientation;

    float mFOV;
    float mNearClip;
    float mFarClip;
};

} // namespace Piksela