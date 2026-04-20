#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    float distance = 16.0f; // Increased backwards distance
    float yaw = -90.0f;
    float pitch = 35.0f; // Pitch camera slightly further down
    bool isOrthographic = false;
    float orthoSize = 10.0f;

    glm::mat4 GetViewMatrix(glm::vec3 target) {
        // Convert angles to a 3D position
        float x = target.x + distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        float y = target.y + distance * sin(glm::radians(pitch));
        float z = target.z + distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        
        return glm::lookAt(glm::vec3(x, y, z), target, glm::vec3(0, 1, 0));
    }
};