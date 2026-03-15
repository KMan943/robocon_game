#include "Physics.hpp"
#include <iostream>

Physics::Physics() : score(0) {}

// Simple AABB (Axis-Aligned Bounding Box) Collision Detection
bool Physics::checkCollision(glm::vec3 pos1, glm::vec3 size1, glm::vec3 pos2, glm::vec3 size2) {
    // Check if the boxes overlap on all three axes
    bool collisionX = pos1.x + size1.x >= pos2.x && pos2.x + size2.x >= pos1.x;
    bool collisionY = pos1.y + size1.y >= pos2.y && pos2.y + size2.y >= pos1.y;
    bool collisionZ = pos1.z + size1.z >= pos2.z && pos2.z + size2.z >= pos1.z;
    
    return collisionX && collisionY && collisionZ;
}

void Physics::updateRobotPosition(glm::vec3 &robotPos, GLFWwindow* window, float deltaTime) {
    float speed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) robotPos.z -= speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) robotPos.z += speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) robotPos.x -= speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) robotPos.x += speed;
}

void Physics::handleScoring(glm::vec3 robotPos, glm::vec3 &objectPos, float &currentScore) {
    // Assume robot size is 1.0f and object size is 0.5f
    if (checkCollision(robotPos, glm::vec3(1.0f), objectPos, glm::vec3(0.5f))) {
        currentScore++;
        std::cout << "Score: " << currentScore << std::endl;
        
        // Respawn object at a new random position within arena bounds (-5 to 5)
        objectPos.x = (rand() % 10) - 5;
        objectPos.z = (rand() % 10) - 5;
    }
}
