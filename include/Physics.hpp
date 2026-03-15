#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Physics {
public:
    int score;

    Physics();

    // Movement logic
    void updateRobotPosition(glm::vec3 &robotPos, GLFWwindow* window, float deltaTime);

    // Collision & Scoring logic
    bool checkCollision(glm::vec3 pos1, glm::vec3 size1, glm::vec3 pos2, glm::vec3 size2);
    void handleScoring(glm::vec3 robotPos, glm::vec3 &objectPos, float &currentScore);
};

#endif
