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
    float yaw = 0.0f; // Rotation angle in degrees
    
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
        // Base translation and rotation for the player
        glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), pos);
        baseModel = glm::rotate(baseModel, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Colors for Wall-E
        glm::vec3 bodyColor = glm::vec3(0.9f, 0.7f, 0.1f); // Yellowish orange
        glm::vec3 treadColor = glm::vec3(0.2f, 0.2f, 0.2f); // Dark Grey
        glm::vec3 neckColor = glm::vec3(0.5f, 0.5f, 0.5f); // Grey
        glm::vec3 eyeColor = glm::vec3(0.1f, 0.1f, 0.1f); // Dark Grey
        glm::vec3 lensColor = glm::vec3(0.6f, 0.8f, 1.0f); // Light Blue

        // 1. Body (Torso)
        glm::mat4 modelBody = glm::translate(baseModel, glm::vec3(0.0f, -0.05f, 0.0f));
        modelBody = glm::scale(modelBody, glm::vec3(0.45f, 0.45f, 0.45f));
        shader.setMat4("model", modelBody);
        shader.setVec3("objectColor", bodyColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 2. Left Tread
        glm::mat4 modelLTread = glm::translate(baseModel, glm::vec3(-0.25f, -0.2f, 0.0f));
        modelLTread = glm::scale(modelLTread, glm::vec3(0.15f, 0.4f, 0.6f));
        shader.setMat4("model", modelLTread);
        shader.setVec3("objectColor", treadColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 3. Right Tread
        glm::mat4 modelRTread = glm::translate(baseModel, glm::vec3(0.25f, -0.2f, 0.0f));
        modelRTread = glm::scale(modelRTread, glm::vec3(0.15f, 0.4f, 0.6f));
        shader.setMat4("model", modelRTread);
        shader.setVec3("objectColor", treadColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 4. Neck
        glm::mat4 modelNeck = glm::translate(baseModel, glm::vec3(0.0f, 0.25f, 0.0f));
        modelNeck = glm::scale(modelNeck, glm::vec3(0.1f, 0.3f, 0.1f));
        shader.setMat4("model", modelNeck);
        shader.setVec3("objectColor", neckColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 5. Left Eye
        glm::mat4 modelLEye = glm::translate(baseModel, glm::vec3(-0.12f, 0.45f, 0.05f));
        modelLEye = glm::scale(modelLEye, glm::vec3(0.18f, 0.15f, 0.2f));
        shader.setMat4("model", modelLEye);
        shader.setVec3("objectColor", eyeColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 6. Right Eye
        glm::mat4 modelREye = glm::translate(baseModel, glm::vec3(0.12f, 0.45f, 0.05f));
        modelREye = glm::scale(modelREye, glm::vec3(0.18f, 0.15f, 0.2f));
        shader.setMat4("model", modelREye);
        shader.setVec3("objectColor", eyeColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 7. Left Lens
        glm::mat4 modelLLens = glm::translate(baseModel, glm::vec3(-0.12f, 0.45f, 0.16f));
        modelLLens = glm::scale(modelLLens, glm::vec3(0.1f, 0.1f, 0.02f));
        shader.setMat4("model", modelLLens);
        shader.setVec3("objectColor", lensColor); 
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 8. Right Lens
        glm::mat4 modelRLens = glm::translate(baseModel, glm::vec3(0.12f, 0.45f, 0.16f));
        modelRLens = glm::scale(modelRLens, glm::vec3(0.1f, 0.1f, 0.02f));
        shader.setMat4("model", modelRLens);
        shader.setVec3("objectColor", lensColor); 
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

    Coin() : Entity(glm::vec3(0.0f), glm::vec3(0.4f), glm::vec3(1.0f, 0.84f, 0.0f)) {} // Gold base color

    void Update(float dt) {
        if (isActive) {
            activeTime += dt;
            rotationAngle += 180.0f * dt; // Rotate 180 degrees per second
            if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;
        }
    }

    void Draw(Shader& shader) override {
        if (isActive) {
            glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), pos);
            
            // Spin the coin around the global Y axis
            baseModel = glm::rotate(baseModel, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            
            // Tilt it up so it stands vertically (rotate around X)
            baseModel = glm::rotate(baseModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            // Draw the outer ring (12-sided disk)
            shader.setVec3("objectColor", color); // bright gold
            for(int i = 0; i < 6; i++) {
                glm::mat4 model = glm::rotate(baseModel, glm::radians(i * 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.4f, 0.05f, 0.107f));
                shader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            
            // Draw the inner engraved core (slightly thicker, smaller diameter, darker gold)
            shader.setVec3("objectColor", glm::vec3(0.85f, 0.65f, 0.0f)); // Darker gold
            for(int i = 0; i < 6; i++) {
                glm::mat4 model = glm::rotate(baseModel, glm::radians(i * 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(0.3f, 0.06f, 0.08f));
                shader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }
    }
};