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
    unsigned int bodyTexture = 0;
    unsigned int eyeTexture = 0;


    
    // the Coyote time problem
    float coyoteCounter = 0.0f;
    const float coyoteTime = 0.15f;
    bool grounded = false;

    Player(glm::vec3 p) : Entity(p, glm::vec3(1.2f), glm::vec3(1.0f, 0.5f, 0.0f)) {}

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
        baseModel = glm::scale(baseModel, size); // Scale the visual parts
        
        // Colors for Wall-E
        glm::vec3 bodyColor = glm::vec3(0.9f, 0.7f, 0.1f); // Yellowish orange
        glm::vec3 treadColor = glm::vec3(0.2f, 0.2f, 0.2f); // Dark Grey
        glm::vec3 neckColor = glm::vec3(0.5f, 0.5f, 0.5f); // Grey
        glm::vec3 eyeColor = glm::vec3(0.1f, 0.1f, 0.1f); // Dark Grey
        glm::vec3 lensColor = glm::vec3(0.6f, 0.8f, 1.0f); // Light Blue

        // 1. Body (Torso)
        if (bodyTexture != 0) {
            shader.setInt("useTexture", 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, bodyTexture);
            shader.setVec3("objectColor", glm::vec3(1.0f)); // Neutral color for texture
        } else {
            shader.setInt("useTexture", 0);
            shader.setVec3("objectColor", bodyColor);
        }
        glm::mat4 modelBody = glm::translate(baseModel, glm::vec3(0.0f, -0.05f, 0.0f));
        modelBody = glm::scale(modelBody, glm::vec3(0.45f, 0.45f, 0.45f));
        shader.setMat4("model", modelBody);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.setInt("useTexture", 0); // Reset for other parts


        // 2. Left Tread
        shader.setInt("useTexture", 0);
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
        if (eyeTexture != 0) {
            shader.setInt("useTexture", 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, eyeTexture);
            shader.setVec3("objectColor", glm::vec3(1.0f));
        } else {
            shader.setInt("useTexture", 0);
            shader.setVec3("objectColor", lensColor);
        }
        glm::mat4 modelLLens = glm::translate(baseModel, glm::vec3(-0.12f, 0.45f, 0.16f));
        modelLLens = glm::scale(modelLLens, glm::vec3(0.1f, 0.1f, 0.02f));
        shader.setMat4("model", modelLLens);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 8. Right Lens
        if (eyeTexture != 0) {
            shader.setInt("useTexture", 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, eyeTexture);
            shader.setVec3("objectColor", glm::vec3(1.0f));
        } else {
            shader.setInt("useTexture", 0);
            shader.setVec3("objectColor", lensColor);
        }
        glm::mat4 modelRLens = glm::translate(baseModel, glm::vec3(0.12f, 0.45f, 0.16f));
        modelRLens = glm::scale(modelRLens, glm::vec3(0.1f, 0.1f, 0.02f));
        shader.setMat4("model", modelRLens);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.setInt("useTexture", 0);

    }
};

// --- THE PLATFORM TILE ---
class Tile : public Entity {
public:
    bool isHole;
    glm::vec3 vel = glm::vec3(0.0f); // Used for moving platforms

    Tile(glm::vec3 p, bool hole) 
        : Entity(p, glm::vec3(2.0f, 0.2f, 2.0f), glm::vec3(0.2f, 0.7f, 0.2f)), isHole(hole) {
        if (isHole) 
            color = glm::vec3(0.05f); // Make holes look dark
    }

    virtual void Update(float dt) {}

    void Draw(Shader& shader) override {
        if (!isHole) { // We don't draw the hole, just the gap!
            shader.setInt("useTexture", 1);
            shader.setVec3("objectColor", color);
            shader.setMat4("model", GetModelMatrix());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
};

class MovingPlatform : public Tile {
public:
    glm::vec3 posA;
    glm::vec3 posB;
    float speed;
    int direction = 1;

    MovingPlatform(glm::vec3 a, glm::vec3 b, float s) : Tile(a, false), posA(a), posB(b), speed(s) {
        color = glm::vec3(0.8f, 0.4f, 0.1f); // Orange moving platform!
    }

    void Update(float dt) override {
        glm::vec3 dirVec = glm::normalize(posB - posA);
        glm::vec3 velocity = dirVec * speed * (float)direction;
        pos += velocity * dt;
        vel = velocity;
        
        float distA = glm::length(pos - posA);
        float distB = glm::length(pos - posB);
        float totalDist = glm::length(posB - posA);
        
        if (direction == 1 && distA >= totalDist) {
            direction = -1;
            pos = posB;
        } else if (direction == -1 && distB >= totalDist) {
            direction = 1;
            pos = posA;
        }
    }
};

// --- THE COIN ---
class Coin : public Entity {
public:
    bool isActive = false;
    float activeTime = 0.0f;
    float rotationAngle = 0.0f;

    Coin() : Entity(glm::vec3(0.0f), glm::vec3(0.6f), glm::vec3(1.0f, 0.84f, 0.0f)) {} // Gold base color

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
            
            // Visual scale up matching the new size ratio (0.6 / 0.4 = 1.5)
            baseModel = glm::scale(baseModel, glm::vec3(1.5f));

            shader.setInt("useTexture", 1);
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

// --- THE OBSTACLE ---
class Obstacle : public Entity {
public:
    Obstacle(glm::vec3 p) : Entity(p, glm::vec3(0.4f), glm::vec3(0.9f, 0.1f, 0.1f)) {} // Hitbox is tiny (0.4), visual is larger

    void Draw(Shader& shader) override {
        glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), pos);
        // We use a fixed visual scale of 1.0f here and apply it to parts
        // to decouple from the 0.4f hitbox size
        
        shader.setInt("useTexture", 0);
        
        // Core of the mine (Visual size 0.9) - Textured!
        shader.setInt("useTexture", 1);
        shader.setVec3("objectColor", glm::vec3(0.5f, 0.5f, 0.5f)); 
        glm::mat4 modelCore = glm::scale(baseModel, glm::vec3(0.9f, 0.9f, 0.9f));
        shader.setMat4("model", modelCore);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Red Spikes (Visual size 1.6) - Not textured
        shader.setInt("useTexture", 0);
        shader.setVec3("objectColor", color);
        
        // X spike
        glm::mat4 spikeX = glm::scale(baseModel, glm::vec3(1.6f, 0.2f, 0.2f));
        shader.setMat4("model", spikeX);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // Y spike
        glm::mat4 spikeY = glm::scale(baseModel, glm::vec3(0.2f, 1.6f, 0.2f));
        shader.setMat4("model", spikeY);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // Z spike
        glm::mat4 spikeZ = glm::scale(baseModel, glm::vec3(0.2f, 0.2f, 1.6f));
        shader.setMat4("model", spikeZ);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};

// --- THE GOAL ---
class Goal : public Entity {
public:
    float rotationAngle = 0.0f;

    Goal(glm::vec3 p) : Entity(p, glm::vec3(1.5f, 3.0f, 1.5f), glm::vec3(0.2f, 0.9f, 0.9f)) {} // Cyan portal/marker

    void Update(float dt) {
        rotationAngle += 90.0f * dt; // Rotate portal
        if(rotationAngle >= 360.0f) rotationAngle -= 360.0f;
    }

    void Draw(Shader& shader) override {
        // Base translation to the center of the tile
        glm::mat4 baseModel = glm::translate(glm::mat4(1.0f), pos);
        
        shader.setInt("useTexture", 0);
        // Draw the Pole - Centered at baseModel (no translation needed)
        shader.setVec3("objectColor", glm::vec3(0.9f, 0.9f, 0.9f));
        // Use individual scaling to avoid affecting the flag's coordinate system
        glm::mat4 modelPole = glm::scale(baseModel, glm::vec3(size.x * 0.1f, size.y, size.z * 0.1f));
        shader.setMat4("model", modelPole);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw the Flag - Rotating around the pole!
        shader.setInt("useTexture", 1);
        shader.setVec3("objectColor", color);
        
        // 1. Start from base (at pole center)
        // 2. Rotate around Y axis
        glm::mat4 modelFlag = glm::rotate(baseModel, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        // 3. Translate flag away from the pole (outwards on X)
        modelFlag = glm::translate(modelFlag, glm::vec3(size.x * 0.45f, size.y * 0.35f, 0.0f));
        // 4. Scale the flag
        modelFlag = glm::scale(modelFlag, glm::vec3(size.x * 0.8f, size.y * 0.3f, size.z * 0.05f));
        
        shader.setMat4("model", modelFlag);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.setInt("useTexture", 0);
    }
};