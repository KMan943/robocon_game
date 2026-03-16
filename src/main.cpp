#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "Physics.hpp"
#include "iostream"


// Global state for mouse
Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    camera.yaw   += (xpos - lastX) * 0.1f;
    camera.pitch -= (ypos - lastY) * 0.1f; // Inverted Y
    if (camera.pitch > 89.0f) camera.pitch = 89.0f;
    if (camera.pitch < -89.0f) camera.pitch = -89.0f;
    lastX = xpos; lastY = ypos;
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Robocon 3D", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Lock mouse
    glfwSetCursorPosCallback(window, mouse_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // 36 Vertices for a 1x1x1 Cube with Normals (for lighting)
    float vertices[] = {
        // positions          // normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // IMPORTANT: Bind this VAO before your render loop
    glBindVertexArray(VAO);

    glEnable(GL_DEPTH_TEST);

    Shader shader("assets/shaders/vertex_shader.glsl", "assets/shaders/fragment_shader.glsl");
    Player robot(glm::vec3(0.0f, 5.0f, 0.0f));
    
    // Create tiled floor with a hole
    std::vector<Tile*> floor;
    for(int x = -3; x <= 3; x++) {
        for(int z = -3; z <= 3; z++) {
            bool hole = (x == 0 && z == 0); // Hole in the middle
            floor.push_back(new Tile(glm::vec3(x * 2.1f, 0, z * 2.1f), hole));
        }
    }

    while (!glfwWindowShouldClose(window)) {
        float dt = 0.016f; // Standard frame time
        
        // 1. INPUT (WASD)
        float speed = 5.0f * dt;
        if(glfwGetKey(window, GLFW_KEY_W)) robot.pos.z -= speed;
        if(glfwGetKey(window, GLFW_KEY_S)) robot.pos.z += speed;
        if(glfwGetKey(window, GLFW_KEY_A)) robot.pos.x -= speed;
        if(glfwGetKey(window, GLFW_KEY_D)) robot.pos.x += speed;

        // 2. PHYSICS (Fall through hole)
        Physics::UpdatePhysics(robot, floor, dt); // Call the centralized physics function
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            robot.pos = glm::vec3(0.0f, 5.0f, 0.0f);
            robot.vel = glm::vec3(0.0f);
        }
        robot.pos += robot.vel * dt;

        if (robot.pos.y < -10.0f) {
            std::cout << "GAME OVER: You fell into the void!" << std::endl;
            
            // Reset State
            robot.pos = glm::vec3(0.0f, 5.0f, 0.0f);
            robot.vel = glm::vec3(0.0f);
            
            // Future: Here is where you would decrease 'Lives' or show a UI screen
        }

        // for(auto t : floor) {
        //     if(!t->isHole && abs(robot.pos.x - t->pos.x) < 1.0f && abs(robot.pos.z - t->pos.z) < 1.0f) {
        //         if(robot.pos.y < 0.5f) { robot.pos.y = 0.5f; robot.vel.y = 0; }
        //     }
        // }

        // 3. RENDER
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.setMat4("projection", glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f));
        shader.setMat4("view", camera.GetViewMatrix(robot.pos));

        robot.Draw(shader);
        for(auto t : floor) t->Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}