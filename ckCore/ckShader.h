#pragma once
#include "ckObject.h"
#include "stdInclude.h"
#include "glad/glad.h"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

class ckShader : ckObject
{
private:
    void checkShaderCompiling(GLuint shader) const;
    void checkShaderProgramCompiling(GLuint shaderProgram) const;

public:
    unsigned int ID; // 程序ID

    ckShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    ~ckShader();

    void use() const;
    void setParameter(const std::string& name, bool value) const;
    void setParameter(const std::string& name, int value) const;
    void setParameter(const std::string& name, float value) const;
    void setParameter(const std::string& name, glm::vec3 value) const;
    void setParameter(const std::string& name, glm::vec2 value) const;
    void setParameter(const std::string& name, glm::mat4 value) const;
};
