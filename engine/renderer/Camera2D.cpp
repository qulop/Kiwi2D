#include "Camera2D.hpp"

#include <glm/gtc/matrix_transform.hpp>


namespace Kiwi {
    Camera2D::Camera2D(f32 left, f32 right, f32 bottom, f32 top) :
        m_projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)),
        m_viewMatrix(1.0f)
    {
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
    }

    void Camera2D::SetProjection(f32 left, f32 right, f32 bottom, f32 top) {
        m_projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
    }

    void Camera2D::RecalculateViewMatrix() {
        const glm::mat4 transform =
            glm::translate(glm::mat4(1.0f), glm::vec3(m_position.x, m_position.y, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));

        m_viewMatrix = glm::inverse(transform);
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
    }
}
