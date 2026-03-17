#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.hpp"

class Entity {
public:
    glm::vec3 pos;
    glm::vec3 size;
    glm::vec3 color;

    Entity(glm::vec3 p, glm::vec3 s, glm::vec3 c) : pos(p), size(s), color(c) {}
    virtual ~Entity() {}

    // A utility to get the "Model" matrix (Position + Scale)
    glm::mat4 GetModelMatrix() {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, size);
        return model;
    }

    virtual void Draw(Shader& shader) = 0; // "Pure Virtual" - must be defined by children
};

// --- THE ROBOT ---
class Player : public Entity {
public:
    glm::vec3 vel = glm::vec3(0.0f);
    
    // the Coyote time problem
    float coyoteCounter = 0.0f;
    const float coyoteTime = 0.15f;
    bool grounded = false;

    Player(glm::vec3 p) : Entity(p, glm::vec3(0.8f), glm::vec3(1.0f, 0.5f, 0.0f)) {}

    void Update(float dt){
        // decreasing the counter every frame
        if(!grounded){
            coyoteCounter -= dt;
        }
        else{
            coyoteCounter = coyoteTime;
        }
        pos += vel * dt;
    }

    void Draw(Shader& shader) override {
        shader.setVec3("objectColor", color);
        shader.setMat4("model", GetModelMatrix());
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};

// --- THE PLATFORM TILE ---
class Tile : public Entity {
public:
    bool isHole;
    Tile(glm::vec3 p, bool hole) 
        : Entity(p, glm::vec3(2.0f, 0.2f, 2.0f), glm::vec3(0.2f, 0.7f, 0.2f)), isHole(hole) {
        if (isHole) 
            color = glm::vec3(0.05f); // Make holes look dark
        
    }

    void Draw(Shader& shader) override {
        if (!isHole) { // We don't draw the hole, just the gap!
            shader.setVec3("objectColor", color);
            shader.setMat4("model", GetModelMatrix());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
};

// --- THE COIN ---
class Coin : public Entity {
public:
    bool isActive = false;
    float activeTime = 0.0f;
    float rotationAngle = 0.0f;

    Coin() : Entity(glm::vec3(0.0f), glm::vec3(0.4f), glm::vec3(1.0f, 1.0f, 0.0f)) {} // Yellow, smaller cube

    void Update(float dt) {
        if (isActive) {
            activeTime += dt;
            rotationAngle += 90.0f * dt; // Rotate 90 degrees per second
            if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;
        }
    }

    void Draw(Shader& shader) override {
        if (isActive) {
            shader.setVec3("objectColor", color);
            
            // Custom model matrix to include rotation
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            // Rotate around Y axis
            model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, size);
            
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
};