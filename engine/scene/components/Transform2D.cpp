#include "Transform2D.hpp"

#include <glm/gtc/matrix_transform.hpp>


namespace Kiwi {
    glm::mat4 Transform2D::GetModelMatrix() const {
        const glm::mat4 translation = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(position.x, position.y, 0.0f)
        );

        const glm::mat4 rotationMatrix = glm::rotate(
            glm::mat4(1.0f),
            rotation,
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        const glm::mat4 scaling = glm::scale(
            glm::mat4(1.0f),
            glm::vec3(scale.x, scale.y, 1.0f)
        );

        return translation * rotationMatrix * scaling;
    }
}
