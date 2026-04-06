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
    LEVEL_SELECT,
    PLAYING,
    PAUSED,
    GAMEOVER,
    WIN
};
GameState gameState = GameState::MENU;
int currentLevel = 1;
int maxLevel = 3;

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

void loadLevel(int level, std::vector<Tile*>& floor, std::vector<Obstacle*>& obstacles, std::vector<Coin*>& coins, Goal*& goal, Player& robot, float& levelTimer, int& coinsCollectedLevel, int& requiredCoins) {
    for(auto t : floor) delete t;
    floor.clear();
    for(auto o : obstacles) delete o;
    obstacles.clear();
    for(auto c : coins) delete c;
    coins.clear();
    if (goal) { delete goal; goal = nullptr; }

    robot.pos = glm::vec3(0.0f, 5.0f, 0.0f);
    robot.vel = glm::vec3(0.0f);
    robot.yaw = 180.0f; // Rotate robot by 180 degrees at spawn
    camera.yaw = 0.0f;
    camera.pitch = -20.0f; // Reset look down

    coinsCollectedLevel = 0;

    if (level == 3) {
        levelTimer = 90.0f;
    } else {
        levelTimer = 120.0f;
    }

    if (level == 1) {
        requiredCoins = 5;
        for(int x = -3; x <= 3; x++) {
            for(int z = -12; z <= 2; z++) {
                bool hole = (x == 0 && z == -4) || (x == 1 && z == -4) || (x == -1 && z == -4);
                floor.push_back(new Tile(glm::vec3(x * 2.1f, 0, z * 2.1f), hole));
            }
        }
        obstacles.push_back(new Obstacle(glm::vec3(-2.1f, 1.0f, -8.0f)));
        obstacles.push_back(new Obstacle(glm::vec3(2.1f, 1.0f, -8.0f)));
        obstacles.push_back(new Obstacle(glm::vec3(0.0f, 1.0f, -14.0f)));
        
        // Setup coin pool for Level 1
        for(int i=0; i<5; i++) {
            coins.push_back(new Coin()); 
            coins.back()->isActive = false;
        }

        goal = new Goal(glm::vec3(0.0f, 1.5f, -22.0f));
    } else if (level == 2) {
        requiredCoins = 3;
        for(int x = -5; x <= 5; x++) {
            for(int z = -20; z <= 3; z++) {
                // Revert to original single-tile gaps
                bool hole = ((x % 2 != 0) && (z % 3 == 0)) || (x == 0 && z == -6) || (x == 1 && z == -7) || (x == -1 && z == -7);
                floor.push_back(new Tile(glm::vec3(x * 2.1f, 0, z * 2.1f), hole));
            }
        }
        obstacles.push_back(new Obstacle(glm::vec3(0.0f, 1.0f, -8.0f)));
        obstacles.push_back(new Obstacle(glm::vec3(-4.2f, 1.0f, -4.2f)));
        obstacles.push_back(new Obstacle(glm::vec3(4.2f, 1.0f, -10.5f)));
        obstacles.push_back(new Obstacle(glm::vec3(-2.1f, 1.0f, -16.0f)));
        obstacles.push_back(new Obstacle(glm::vec3(2.1f, 1.0f, -24.0f)));
        
        // Setup coin pool for Level 2
        for(int i=0; i<4; i++) {
            coins.push_back(new Coin()); 
            coins.back()->isActive = false;
        }

        goal = new Goal(glm::vec3(0.0f, 1.5f, -38.0f));
    } else if (level == 3) {
        requiredCoins = 3;
        
        // Platform 1 (Start)
        for(int x = -2; x <= 2; x++) {
            for(int z = 0; z >= -3; z--) {
                floor.push_back(new Tile(glm::vec3(x * 2.1f, 0.0f, z * 2.1f), false));
            }
        }
        
        // Two slow moving tiles spacing the gap
        floor.push_back(new MovingPlatform(glm::vec3(-4.0f, 0.0f, -10.0f), glm::vec3(4.0f, 0.0f, -10.0f), 2.0f));
        floor.push_back(new MovingPlatform(glm::vec3(4.0f, 0.0f, -14.0f), glm::vec3(-4.0f, 0.0f, -14.0f), 2.0f));

        // Platform 2 (Middle)
        for(int x = -2; x <= 2; x++) {
            for(int z = -8; z >= -11; z--) {
                floor.push_back(new Tile(glm::vec3(x * 2.1f, 0.0f, z * 2.1f), false));
            }
        }

        // Staircase to Platform 3
        floor.push_back(new Tile(glm::vec3(0.0f, 1.5f, -26.0f), false));
        floor.push_back(new Tile(glm::vec3(0.0f, 3.0f, -29.0f), false));
        floor.push_back(new Tile(glm::vec3(0.0f, 4.5f, -32.0f), false));
        floor.push_back(new Tile(glm::vec3(0.0f, 6.0f, -35.0f), false));
        floor.push_back(new Tile(glm::vec3(0.0f, 7.5f, -38.0f), false));

        // Platform 3 (Finish)
        for(int x = -2; x <= 2; x++) {
            for(int z = -19; z >= -22; z--) {
                floor.push_back(new Tile(glm::vec3(x * 2.1f, 9.0f, z * 2.1f), false));
            }
        }
        
        // Obstacles (Mines)
        // One mine on Platform 2, offset to the side
        obstacles.push_back(new Obstacle(glm::vec3(-2.1f, 1.0f, -20.0f)));
        
        // One mine on Platform 3, kept away from the flag spawn point
        obstacles.push_back(new Obstacle(glm::vec3(2.1f, 10.0f, -43.0f))); 

        // Setup coin pool for Level 3
        for(int i=0; i<4; i++) {
            coins.push_back(new Coin()); 
            coins.back()->isActive = false;
        }
        
        // The Goal is on the final platform
        goal = new Goal(glm::vec3(0.0f, 10.5f, -44.0f));
    }
}

// Function to reset the game
void reset_game(Player& robot, int& score, int& current_lives, std::vector<Tile*>& floor, std::vector<Obstacle*>& obstacles, std::vector<Coin*>& coins, Goal*& goal, int startingLevel, float& levelTimer, int& coinsCollectedLevel, int& requiredCoins) {
    score = 0;
    current_lives = 5;
    currentLevel = startingLevel;
    loadLevel(currentLevel, floor, obstacles, coins, goal, robot, levelTimer, coinsCollectedLevel, requiredCoins);
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
    Shader shadowShader("assets/shaders/shadow_vertex.glsl", "assets/shaders/shadow_fragment.glsl");
    Shader bgShader("assets/shaders/bg_vertex.glsl", "assets/shaders/bg_fragment.glsl");
    Shader spaceShader("assets/shaders/space_vertex.glsl", "assets/shaders/space_fragment.glsl");

    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shader.use();
    shader.setInt("shadowMap", 1);

    Player robot(glm::vec3(0.0f, 5.0f, 0.0f));
    
    int score = 0;
    float levelTimer = 120.0f;
    int coinsCollectedLevel = 0;
    int requiredCoins = 0;
    
    float coinSpawnTimer = 0.0f;
    const float coinSpawnInterval = 3.0f;
    const float coinActiveDuration = 10.0f;

    // Game Entities
    std::vector<Tile*> floor;
    std::vector<Obstacle*> obstacles;
    std::vector<Coin*> coins;
    Goal* goal = nullptr;
    
    // Load the first level visually for the menu background
    loadLevel(currentLevel, floor, obstacles, coins, goal, robot, levelTimer, coinsCollectedLevel, requiredCoins);

    // Toggle variables
    bool escapePressed = false;

    auto renderScene = [&](Shader& s) {
        glBindVertexArray(VAO);
        robot.Draw(s);
        for(auto c : coins) if(c->isActive) c->Draw(s);
        for(auto t : floor) t->Draw(s);
        for(auto o : obstacles) o->Draw(s);
        if(goal && coinsCollectedLevel >= requiredCoins) goal->Draw(s);
    };

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
            
            robot.Update(dt);
            Physics::UpdatePhysics(robot, floor, dt); 
            
            for (auto t : floor) {
                 t->Update(dt);
            }

            // Timer depletion
            levelTimer -= dt;
            if (levelTimer <= 0.0f) {
                robot.pos.y = -20.0f; // Force a respawn
            }

            // Coin Spawning
            coinSpawnTimer += dt;
            if (coinSpawnTimer >= coinSpawnInterval) {
                coinSpawnTimer = 0.0f;
                std::vector<Tile*> validTiles;
                for (auto t : floor) {
                    if (!t->isHole) validTiles.push_back(t);
                }
                if (!validTiles.empty()) {
                    for (auto c : coins) {
                        if (!c->isActive) {
                            int randomIndex = rand() % validTiles.size();
                            c->pos = validTiles[randomIndex]->pos + glm::vec3(0.0f, 1.0f, 0.0f);
                            c->isActive = true;
                            c->activeTime = 0.0f;
                            break; // Spawn one coin and exit loop
                        }
                    }
                }
            }

            // Coin Logic
            for (auto c : coins) {
                if (c->isActive) {
                    c->Update(dt);
                    if (c->activeTime >= coinActiveDuration) {
                        c->isActive = false; // Despawn after duration
                    } else if (Physics::CheckCoinCollision(robot, *c)) {
                        c->isActive = false;
                        score++;
                        coinsCollectedLevel++;
                    }
                }
            }

            // Obstacle & Goal Logic
            for (auto obs : obstacles) {
                if (Physics::CheckObstacleCollision(robot, *obs)) {
                    robot.pos.y = -20.0f; // Trigger fall damage instantly
                }
            }
            if (goal && coinsCollectedLevel >= requiredCoins) {
                goal->Update(dt);
                if (Physics::CheckGoalCollision(robot, *goal)) {
                    currentLevel++;
                    if (currentLevel > maxLevel) {
                        gameState = GameState::WIN;
                        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    } else {
                        loadLevel(currentLevel, floor, obstacles, coins, goal, robot, levelTimer, coinsCollectedLevel, requiredCoins);
                    }
                }
            }
            
            // Fail condition
            if (robot.pos.y < -10.0f) {
                lives--;
                if(lives <= 0) {
                    gameState = GameState::GAMEOVER;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    float savedTimer = levelTimer;
                    int savedCoins = coinsCollectedLevel;
                    loadLevel(currentLevel, floor, obstacles, coins, goal, robot, levelTimer, coinsCollectedLevel, requiredCoins);
                    levelTimer = savedTimer;
                    coinsCollectedLevel = savedCoins;
                }
            }
            
            // Calculate Camera Pos
            float camX = robot.pos.x + camera.distance * cos(glm::radians(camera.pitch)) * cos(glm::radians(camera.yaw));
            float camY = robot.pos.y + camera.distance * sin(glm::radians(camera.pitch));
            float camZ = robot.pos.z + camera.distance * cos(glm::radians(camera.pitch)) * sin(glm::radians(camera.yaw));
            glm::vec3 viewPos(camX, camY, camZ);

            // Light Matrix
            glm::vec3 lightPos(-10.0f, 30.0f, 0.0f); // Fixed ambient sunlight 
            glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 60.0f);
            glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0, 1.0, 0.0));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;

            // 1. Render depth of scene to texture
            shadowShader.use();
            shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            renderScene(shadowShader);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            
            // 2. Render scene as normal
            glViewport(0, 0, screenWidth, screenHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix(robot.pos);

            // Draw Background Sky
            glDisable(GL_DEPTH_TEST);
            bgShader.use();
            glm::mat4 invVP = glm::inverse(projection * view);
            bgShader.setMat4("invViewProj", invVP);
            bgShader.setVec3("cameraPos", viewPos);
            static unsigned int emptyVAO = 0;
            if (emptyVAO == 0) glGenVertexArrays(1, &emptyVAO);
            glBindVertexArray(emptyVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

            // Draw 3D Models
            shader.use();
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);
            shader.setVec3("viewPos", viewPos);
            shader.setVec3("lightPos", lightPos);
            shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            
            renderScene(shader);
            
            // HUD GUI
            ImGui::SetNextWindowPos(ImVec2(10, 10));
            ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
            // Draw realistic-looking HUD text properties
            ImGui::SetWindowFontScale(1.8f);
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "SCORE: %d", score);
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "LIVES: %d", lives);
            
            int timeDisplay = (int)std::max(0.0f, levelTimer);
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "TIME: %ds", timeDisplay);
            if (coinsCollectedLevel >= requiredCoins) {
                ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "TARGET: DONE! REACH GOAL!");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "TARGET: %d / %d COINS", coinsCollectedLevel, requiredCoins);
            }
            ImGui::End();

        } else if (gameState == GameState::PAUSED) {
            // Keep drawing the scene so we can see it in the background
            float camX = robot.pos.x + camera.distance * cos(glm::radians(camera.pitch)) * cos(glm::radians(camera.yaw));
            float camY = robot.pos.y + camera.distance * sin(glm::radians(camera.pitch));
            float camZ = robot.pos.z + camera.distance * cos(glm::radians(camera.pitch)) * sin(glm::radians(camera.yaw));
            glm::vec3 viewPos(camX, camY, camZ);

            glm::vec3 lightPos(-10.0f, 30.0f, 0.0f);
            glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 60.0f);
            glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0, 1.0, 0.0));
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;

            shadowShader.use();
            shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
            renderScene(shadowShader);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, screenWidth, screenHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix(robot.pos);

            glDisable(GL_DEPTH_TEST);
            bgShader.use();
            glm::mat4 invVP = glm::inverse(projection * view);
            bgShader.setMat4("invViewProj", invVP);
            bgShader.setVec3("cameraPos", viewPos);
            static unsigned int emptyVAOP = 0;
            if (emptyVAOP == 0) glGenVertexArrays(1, &emptyVAOP);
            glBindVertexArray(emptyVAOP);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

            shader.use();
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);
            shader.setVec3("viewPos", viewPos);
            shader.setVec3("lightPos", lightPos);
            shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            renderScene(shader);
            
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
            // Animated Space Galaxy Background
            glDisable(GL_DEPTH_TEST);
            spaceShader.use();
            spaceShader.setFloat("uTime", (float)glfwGetTime());
            spaceShader.setVec2("uResolution", glm::vec2(screenWidth, screenHeight));
            static unsigned int spaceMenuVAO = 0;
            if (spaceMenuVAO == 0) glGenVertexArrays(1, &spaceMenuVAO);
            glBindVertexArray(spaceMenuVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

            // Main Start Menu
            ImGui::SetNextWindowPos(ImVec2(screenWidth / 2.0f, screenHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(500, 450));
            ImGui::Begin("ROBOCON 3D", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowFontScale(2.2f);
            
            ImGui::SetCursorPosX(110);
            ImGui::SetCursorPosY(80);
            ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.1f, 1.0f), "WALL-E QUEST");
            
            ImGui::SetWindowFontScale(1.6f);
            ImGui::SetCursorPosX((500 - 200) / 2);
            ImGui::SetCursorPosY(200);
            if (ImGui::Button("START GAME", ImVec2(200, 60))) {
                gameState = GameState::LEVEL_SELECT;
            }
            
            ImGui::SetCursorPosX((500 - 200) / 2);
            ImGui::SetCursorPosY(290);
            if (ImGui::Button("EXIT", ImVec2(200, 60))) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::End();

        } else if (gameState == GameState::LEVEL_SELECT) {
            // Animated Space Galaxy Background
            glDisable(GL_DEPTH_TEST);
            spaceShader.use();
            spaceShader.setFloat("uTime", (float)glfwGetTime());
            spaceShader.setVec2("uResolution", glm::vec2(screenWidth, screenHeight));
            static unsigned int spacelsVAO = 0;
            if (spacelsVAO == 0) glGenVertexArrays(1, &spacelsVAO);
            glBindVertexArray(spacelsVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

            // Level Select Menu
            ImGui::SetNextWindowPos(ImVec2(screenWidth / 2.0f, screenHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(500, 480));
            ImGui::Begin("SELECT LEVEL", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
            ImGui::SetWindowFontScale(2.2f);
            
            ImGui::SetCursorPosX(110);
            ImGui::SetCursorPosY(60);
            ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.1f, 1.0f), "CHOOSE DIFFICULTY");
            
            ImGui::SetWindowFontScale(1.6f);
            ImGui::SetCursorPosX((500 - 300) / 2);
            ImGui::SetCursorPosY(140);
            if (ImGui::Button("LEVEL 1 (EASY)", ImVec2(300, 60))) {
                reset_game(robot, score, lives, floor, obstacles, coins, goal, 1, levelTimer, coinsCollectedLevel, requiredCoins);
                gameState = GameState::PLAYING;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            
            ImGui::SetCursorPosX((500 - 300) / 2);
            ImGui::SetCursorPosY(215);
            if (ImGui::Button("LEVEL 2 (HARD)", ImVec2(300, 60))) {
                reset_game(robot, score, lives, floor, obstacles, coins, goal, 2, levelTimer, coinsCollectedLevel, requiredCoins);
                gameState = GameState::PLAYING;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            
            ImGui::SetCursorPosX((500 - 300) / 2);
            ImGui::SetCursorPosY(290);
            if (ImGui::Button("LEVEL 3 (HARDCORE)", ImVec2(300, 60))) {
                reset_game(robot, score, lives, floor, obstacles, coins, goal, 3, levelTimer, coinsCollectedLevel, requiredCoins);
                gameState = GameState::PLAYING;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            
            ImGui::SetCursorPosX((500 - 200) / 2);
            ImGui::SetCursorPosY(380);
            if (ImGui::Button("BACK", ImVec2(200, 50))) {
                gameState = GameState::MENU;
            }
            ImGui::End();

        } else if (gameState == GameState::GAMEOVER) {
            // Background Sky for Game Over
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
            glm::vec3 menuCamPos(0.0f, 5.0f, 10.0f);
            glm::mat4 view = glm::lookAt(menuCamPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
            glDisable(GL_DEPTH_TEST);
            bgShader.use();
            glm::mat4 invVP = glm::inverse(projection * view);
            bgShader.setMat4("invViewProj", invVP);
            bgShader.setVec3("cameraPos", menuCamPos);
            static unsigned int goVAO = 0;
            if (goVAO == 0) glGenVertexArrays(1, &goVAO);
            glBindVertexArray(goVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

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
                reset_game(robot, score, lives, floor, obstacles, coins, goal, currentLevel, levelTimer, coinsCollectedLevel, requiredCoins);
                gameState = GameState::PLAYING;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            ImGui::End();
        } else if (gameState == GameState::WIN) {
            // Background Sky for Win
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
            glm::vec3 menuCamPos(0.0f, 5.0f, 10.0f);
            glm::mat4 view = glm::lookAt(menuCamPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));
            glDisable(GL_DEPTH_TEST);
            bgShader.use();
            glm::mat4 invVP = glm::inverse(projection * view);
            bgShader.setMat4("invViewProj", invVP);
            bgShader.setVec3("cameraPos", menuCamPos);
            static unsigned int winVAO = 0;
            if (winVAO == 0) glGenVertexArrays(1, &winVAO);
            glBindVertexArray(winVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            glEnable(GL_DEPTH_TEST);

            // Win Screen
            ImGui::SetNextWindowPos(ImVec2(screenWidth / 2.0f, screenHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(350, 250));
            ImGui::Begin("YOU WIN!", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            ImGui::SetWindowFontScale(2.5f);
            
            ImGui::SetCursorPosX(50);
            ImGui::SetCursorPosY(50);
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "LEVELS CLEARED!");
            
            ImGui::SetWindowFontScale(1.5f);
            ImGui::SetCursorPosX((350 - 200) / 2);
            ImGui::SetCursorPosY(120);
            ImGui::Text("Final Score: %d", score);
            
            ImGui::SetCursorPosX((350 - 200) / 2);
            ImGui::SetCursorPosY(170);
            if (ImGui::Button("MAIN MENU", ImVec2(200, 50))) {
                gameState = GameState::MENU;
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