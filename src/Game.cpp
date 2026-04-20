#include "Game.hpp"
#include "iostream"

Game::Game() : state(GameState::MAIN_MENU) {
    glfwInit();
    // Get Desktop Resolution for Fullscreen
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    screenWidth = mode->width;
    screenHeight = mode->height;

    window = glfwCreateWindow(screenWidth, screenHeight, "Robocon 3D", glfwGetPrimaryMonitor(), NULL);
    glfwMakeContextCurrent(window);
    
    glfwSetWindowUserPointer(window, this);
    
    glfwSetCursorPosCallback(window, Game::MouseCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // Setup Style (Dark mode looks best for Robocon)
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    InitRenderData();
    glEnable(GL_DEPTH_TEST);

    gameShader = new Shader("assets/shaders/vertex_shader.glsl", "assets/shaders/fragment_shader.glsl");
    robot = new Player(glm::vec3(0.0f, 5.0f, 0.0f));
    
    // Create Floor
    for(int x = -3; x <= 3; x++) {
        for(int z = -3; z <= 3; z++) {
            bool hole = (x == 1 && z == 1); 
            floor.push_back(new Tile(glm::vec3(x * 2.1f, 0, z * 2.1f), hole));
        }
    }
}

void Game::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // Get the Game instance back from the window pointer
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

    // Only rotate if we are playing
    if (game->state != GameState::PLAYING) return;

    if (game->firstMouse) {
        game->lastX = xpos;
        game->lastY = ypos;
        game->firstMouse = false;
    }

    float xoffset = xpos - game->lastX;
    float yoffset = game->lastY - ypos; // reversed since y-coordinates go from bottom to top
    game->lastX = xpos;
    game->lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    game->camera.yaw   += xoffset;
    game->camera.pitch += yoffset;

    // Constraint: prevent screen flipping
    if (game->camera.pitch > 89.0f) game->camera.pitch = 89.0f;
    if (game->camera.pitch < -89.0f) game->camera.pitch = -89.0f;
}

void Game::Run() {
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // float dt = 0.016f;

        ProcessInput(dt);
        if (state == GameState::PLAYING) Update(dt);
        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

Game::~Game() {

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 1. Clean up the pointers we created with 'new'
    delete gameShader;
    delete robot;

    // 2. Clean up the vector of Tile pointers
    for (auto t : floor) {
        delete t;
    }
    floor.clear();

    // 3. Clean up OpenGL buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // 4. Shutdown GLFW
    glfwTerminate();
}

void Game::RenderUI() {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. MAIN MENU
    if (state == GameState::MAIN_MENU) {
        ImGui::SetNextWindowPos(ImVec2(screenWidth/2 - 150, screenHeight/2 - 100));
        ImGui::SetNextWindowSize(ImVec2(300, 200));
        ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        
        ImGui::SetCursorPosX(100);
        ImGui::Text("ROBOCON 2026");
        ImGui::Separator();

        ImGui::Text("MAIN MENU");
        ImGui::Separator();

        if (ImGui::Button("PLAY GAME", ImVec2(280, 50))) {
            this->lives = 5;
            state = GameState::PLAYING;
        }
        if (ImGui::Button("QUIT", ImVec2(280, 50))) {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::End();
    }

    // 2. PAUSE MENU
    if (state == GameState::PAUSED) {
        ImGui::SetNextWindowPos(ImVec2(screenWidth/2 - 100, screenHeight/2 - 75));
        ImGui::SetNextWindowSize(ImVec2(200, 150));
        ImGui::Begin("Paused", nullptr, ImGuiWindowFlags_NoDecoration);

        ImGui::Text("ROBOCON 2026");
        ImGui::Separator();

        ImGui::Text("PAUSED");
        ImGui::Separator();

        if (ImGui::Button("RESUME", ImVec2(180, 40))) {
            state = GameState::PLAYING;
        }
        if (ImGui::Button("QUIT TO MENU", ImVec2(180, 40))) {
            state = GameState::MAIN_MENU;
            this->lives = 5;
            ResetGame();
        }
        ImGui::End();
    }

    if (state == GameState::PLAYING) {
        // Transparent overlay for the HUD
        ImGui::SetNextWindowPos(ImVec2(20, 20));
        ImGui::Begin("HUD", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "LIVES: %d", lives);
        
        if (ImGui::Button("Quick Reset (R)")) {
            ResetGame();
        }
        
        ImGui::End();
    }

    if (state == GameState::GAME_OVER) {
        ImGui::SetNextWindowPos(ImVec2(screenWidth/2 - 100, screenHeight/2 - 75));
        ImGui::SetNextWindowSize(ImVec2(200, 150));
        ImGui::Begin("Game Over", nullptr, ImGuiWindowFlags_NoDecoration);

        ImGui::Text("ROBOCON 2026");
        ImGui::Separator();

        ImGui::Text("GAME OVER!!!");
        ImGui::Separator();

        if (ImGui::Button("GO TO MAIN MENU", ImVec2(180, 40))) {
            state = GameState::MAIN_MENU;
            ResetGame();
        }
        if (ImGui::Button("QUIT", ImVec2(180, 40))) {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::End();
    }

    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::ProcessInput(float dt) {
    static bool escWasPressed = false;
    bool escIsPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    // 1. Toggle Pause Logic
    if (escIsPressed && !escWasPressed) {
        if (state == GameState::PLAYING) state = GameState::PAUSED;
        else if (state == GameState::PAUSED) state = GameState::PLAYING;
    }
    escWasPressed = escIsPressed;

    if (state == GameState::PLAYING) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        
        // 2. Movement Logic (Input only sets the direction)
        float yawRad = glm::radians(camera.yaw);
        glm::vec3 forward(cos(yawRad), 0, sin(yawRad));
        forward = glm::normalize(forward);
        glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, -1, 0)));
        float speed = 6.0f * dt;

        if(glfwGetKey(window, GLFW_KEY_W)) robot->pos -= forward * speed;
        if(glfwGetKey(window, GLFW_KEY_S)) robot->pos += forward * speed;
        if(glfwGetKey(window, GLFW_KEY_A)) robot->pos -= right * speed;
        if(glfwGetKey(window, GLFW_KEY_D)) robot->pos += right * speed;

        // Jump & Respawn
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) ResetGame();
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && robot->coyoteCounter > 0) {
            robot->vel.y = 8.0f;
            robot->coyoteCounter = 0.0f;
        }
    } 
    else if (state == GameState::GAME_OVER) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            this->lives = 5;            // Reset lives
            this->state = GameState::MAIN_MENU; // Go back to start
            ResetGame();                // Put robot back at start
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true); // Quit game
        }
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) state = GameState::PLAYING;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
    }
}

void Game::Update(float dt) {
    // PHYSICS AND LOGIC GO HERE
    robot->Update(dt);
    Physics::UpdatePhysics(*robot, floor, dt); 
    
    // Death Condition
    if (robot->pos.y < -10.0f) {
        this->lives--; // Ensure 'int lives' is in Game.hpp
        std::cout << "Lives left: " << lives << std::endl;
        
        if(lives <= 0) {
            state = GameState::GAME_OVER;
            std::cout << "Game Over" << std::endl;
        }
        else{
            ResetGame();
        }
    }
}

void Game::Render() {
    // ALL DRAWING GOES HERE
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gameShader->use();
    
    // Use the dynamic screenWidth/Height for the aspect ratio
    float aspect = (float)screenWidth / (float)screenHeight;
    gameShader->setMat4("projection", glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f));
    gameShader->setMat4("view", camera.GetViewMatrix(robot->pos));

    // Draw world
    glBindVertexArray(VAO);
    robot->Draw(*gameShader);
    for(auto t : floor) t->Draw(*gameShader);

    // GUI Overlays
    if (state == GameState::MAIN_MENU) {
        // Draw Main Menu Text/Buttons
    } else if (state == GameState::PAUSED) {
        // Draw "PAUSED - Press ESC to Resume"
    }

    RenderUI();
}


void Game::ResetGame() {
    robot->pos = glm::vec3(0.0f, 5.0f, 0.0f);
    robot->vel = glm::vec3(0.0f);
}

// ... Move your vertex array data/logic into InitRenderData() ...

void Game::InitRenderData() {
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

    // Use class members (this->VAO, this->VBO), don't redeclare them as 'unsigned int' here
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

}