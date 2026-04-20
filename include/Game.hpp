#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "Physics.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum class GameState { MAIN_MENU, PLAYING, PAUSED, GAME_OVER };

class Game {
public:
    Game();
    ~Game();
    void Run();
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

private:
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void HandleKeyCallback(int key, int action);
    void RenderUI();

    // Window & Viewport
    GLFWwindow* window;
    unsigned int screenWidth, screenHeight;
    GameState state;

    // Game Objects
    Shader* gameShader;
    Player* robot;
    std::vector<Tile*> floor;
    Camera camera;

    // Rendering Data
    unsigned int VAO, VBO;
    void InitRenderData();
    void ResetGame();

    //Camera 
    float lastX, lastY;
    bool firstMouse = true;

    //Gave vars
    int lives = 5;
};