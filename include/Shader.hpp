#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use() { glUseProgram(ID); }
    
    // Abstracting the "Matrix" and "Vector" sends
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setFloat(const std::string &name, float value) const;
    void setInt(const std::string &name, int value) const;
};