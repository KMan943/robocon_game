#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "Physics.hpp"
#include <iostream>
#include <string>

// Global state for mouse
Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;
int lives = 5;

// Game State Enum
enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAMEOVER
};
GameState gameState = GameState::MENU;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (gameState != GameState::PLAYING) {
        firstMouse = true; // reset so it doesn't jump when returning to gameplay
        return; 
    }
    
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    camera.yaw   += (xpos - lastX) * 0.1f;
    camera.pitch -= (ypos - lastY) * 0.1f; // Inverted Y
    if (camera.pitch > 89.0f) camera.pitch = 89.0f;
    if (camera.pitch < -89.0f) camera.pitch = -89.0f;
    lastX = xpos; lastY = ypos;
}

// Function to reset the game
void reset_game(Player& robot, int& score, int& current_lives, Coin& coin) {
    robot.pos = glm::vec3(0.0f, 5.0f, 0.0f);
    robot.vel = glm::vec3(0.0f);
    score = 0;
    current_lives = 5;
    coin.isActive = false;
}

int main() {
    glfwInit();
    
    // Get the primary monitor and its resolution
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    // Create fullscreen window
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Robocon 3D", monitor, NULL);
    glfwMakeContextCurrent(window);
    
    // Start with normal cursor for the menu
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); 
    glfwSetCursorPosCallback(window, mouse_callback);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glViewport(0, 0, screenWidth, screenHeight);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup Dear ImGui style (Realistic / Sleek Dark Mode)
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.95f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.35f, 0.65f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.45f, 0.85f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.25f, 0.55f, 1.00f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

    // 36 Vertices for a 1x1x1 Cube with Normals
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    Shader shader("assets/shaders/vertex_shader.glsl", "assets/shaders/fragment_shader.glsl");
    Player robot(glm::vec3(0.0f, 5.0f, 0.0f));
    Coin coin;
    
    int score = 0;
    float coinSpawnTimer = 0.0f;
    const float coinSpawnInterval = 3.0f;
    const float coinActiveDuration = 10.0f;

    // tiled floor with multiple holes
    std::vector<Tile*> floor;
    for(int x = -7; x <= 7; x++) {
        for(int z = -7; z <= 7; z++) {
            bool hole = (x == 2 && z == 2) || (x == -4 && z == 5) || (x == 5 && z == -3); 
            floor.push_back(new Tile(glm::vec3(x * 2.1f, 0, z * 2.1f), hole));
        }
    }

    // Toggle variables
    bool escapePressed = false;

    while (!glfwWindowShouldClose(window)) {
        float dt = 0.016f; // Standard frame time
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ------------------ GAMEPLAY LOGIC ------------------
        if (gameState == GameState::PLAYING) {
            // Handle Pause Toggle
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                if (!escapePressed) {
                    gameState = GameState::PAUSED;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    escapePressed = true;
                }
            } else {
                escapePressed = false;
            }

            // Movements
            float yawRad = glm::radians(camera.yaw);
            glm::vec3 forward;
            forward.x = cos(yawRad);
            forward.z = sin(yawRad);
            forward = glm::normalize(forward);
            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,-1,0)));

            float turnSpeed = 120.0f * dt;
            float speed = 4.0f * dt;
            
            if(glfwGetKey(window, GLFW_KEY_A)) robot.yaw += turnSpeed;
            if(glfwGetKey(window, GLFW_KEY_D)) robot.yaw -= turnSpeed;

            float robotYawRad = glm::radians(robot.yaw);
            glm::vec3 robotDir;
            robotDir.x = sin(robotYawRad);
            robotDir.y = 0;
            robotDir.z = cos(robotYawRad);
            robotDir = glm::normalize(robotDir);

            if(glfwGetKey(window, GLFW_KEY_W)) robot.pos += robotDir * speed;
            if(glfwGetKey(window, GLFW_KEY_S)) robot.pos -= robotDir * speed;

            // jump
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && robot.coyoteCounter > 0) {
                robot.vel.y = 6.0f;
                robot.coyoteCounter = 0.0f;
            }
            
            // Physics Update
            robot.Update(dt);
            Physics::UpdatePhysics(robot, floor, dt); 
            
            // Coin Logic
            if (!coin.isActive) {
                coinSpawnTimer += dt;
                if (coinSpawnTimer >= coinSpawnInterval) {
                    coinSpawnTimer = 0.0f;
                    std::vector<Tile*> validTiles;
                    for (auto t : floor) {
                        if (!t->isHole) validTiles.push_back(t);
                    }
                    if (!validTiles.empty()) {
                        int randomIndex = rand() % validTiles.size();
                        coin.pos = validTiles[randomIndex]->pos + glm::vec3(0.0f, 1.0f, 0.0f);
                        coin.isActive = true;
                        coin.activeTime = 0.0f;
                    }
                }
            } else {
                coin.Update(dt);
                if (coin.activeTime >= coinActiveDuration) {
                    coin.isActive = false;
                } else if (Physics::CheckCoinCollision(robot, coin)) {
                    coin.isActive = false;
                    score++;
                }
            }
            
            // Fail condition
            if (robot.pos.y < -10.0f) {
                lives--;
                if(lives <= 0) {
                    gameState = GameState::GAMEOVER;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    robot.pos = glm::vec3(0.0f, 5.0f, 0.0f);
                    robot.vel = glm::vec3(0.0f);
                }
            }
            
            // Rendering the 3D scene only when playing or paused
            shader.use();
            shader.setMat4("projection", glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f));
            shader.setMat4("view", camera.GetViewMatrix(robot.pos));
            
            robot.Draw(shader);
            coin.Draw(shader);
            for(auto t : floor) t->Draw(shader);
            
            // HUD GUI
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
            // Draw realistic-looking HUD text properties
            ImGui::SetWindowFontScale(1.8f);
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "SCORE: %d", score);
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "LIVES: %d", lives);
            ImGui::End();

        } else if (gameState == GameState::PAUSED) {
            // Keep drawing the scene so we can see it in the background
            shader.use();
            shader.setMat4("projection", glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f));
            shader.setMat4("view", camera.GetViewMatrix(robot.pos));
            robot.Draw(shader);
            coin.Draw(shader);
            for(auto t : floor) t->Draw(shader);
            
            // Detect un-pause via ESC key
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                if (!escapePressed) {
                    gameState = GameState::PLAYING;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    escapePressed = true;
                }
            } else {
                escapePressed = false;
            }

            ImGui::SetNextWindowPos(ImVec2(screenWidth / 2.0f, screenHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(300, 200));
            ImGui::Begin("PAUSED", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::SetWindowFontScale(1.5f);
            
            // Centered Buttons
            ImGui::SetCursorPosX((300 - 150) / 2);
            ImGui::SetCursorPosY(60);
            if (ImGui::Button("Resume", ImVec2(150, 40))) {
                gameState = GameState::PLAYING;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            ImGui::SetCursorPosX((300 - 150) / 2);
            ImGui::SetCursorPosY(120);
            if (ImGui::Button("Main Menu", ImVec2(150, 40))) {
                gameState = GameState::MENU;
            }
            ImGui::End();

        } else if (gameState == GameState::MENU) {
            // Main Start Menu
            ImGui::SetNextWindowPos(ImVec2(screenWidth / 2.0f, screenHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(400, 350));
            ImGui::Begin("ROBOCON 3D", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowFontScale(2.0f);
            
            ImGui::SetCursorPosX(70);
            ImGui::SetCursorPosY(80);
            ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.1f, 1.0f), "WALL-E QUEST");
            
            ImGui::SetWindowFontScale(1.5f);
            ImGui::SetCursorPosX((400 - 200) / 2);
            ImGui::SetCursorPosY(180);
            if (ImGui::Button("START GAME", ImVec2(200, 50))) {
                reset_game(robot, score, lives, coin);
                gameState = GameState::PLAYING;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            
            ImGui::SetCursorPosX((400 - 200) / 2);
            ImGui::SetCursorPosY(250);
            if (ImGui::Button("EXIT", ImVec2(200, 50))) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::End();

        } else if (gameState == GameState::GAMEOVER) {
            // Game Over Screen
            ImGui::SetNextWindowPos(ImVec2(screenWidth / 2.0f, screenHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(350, 250));
            ImGui::Begin("GAME OVER", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            ImGui::SetWindowFontScale(2.5f);
            
            ImGui::SetCursorPosX(70);
            ImGui::SetCursorPosY(50);
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "YOU DIED!");
            
            ImGui::SetWindowFontScale(1.5f);
            ImGui::SetCursorPosX((350 - 200) / 2);
            ImGui::SetCursorPosY(120);
            ImGui::Text("Final Score: %d", score);
            
            ImGui::SetCursorPosX((350 - 200) / 2);
            ImGui::SetCursorPosY(170);
            if (ImGui::Button("RESTART", ImVec2(200, 50))) {
                reset_game(robot, score, lives, coin);
                gameState = GameState::PLAYING;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            ImGui::End();
        }

        // Render Dear ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}