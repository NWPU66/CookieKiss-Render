#pragma once

#include <cstdint>

#include <string>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glog/logging.h>

namespace ck {

class Shader {
private:
    uint32_t id;

    static void checkShaderCompiling(GLuint shader);
    static void checkShaderProgramCompiling(GLuint shaderProgram);

public:
    Shader(const std::string& vertexShader_path,
           const std::string& fragmentShader_path,
           const std::string& geometryShader_path = "");
    ~Shader();

    Shader(const Shader&)            = default;
    Shader& operator=(const Shader&) = default;
    Shader(Shader&&)                 = default;
    Shader& operator=(Shader&&)      = default;

    void use() const;
    void setParameter(const std::string& name, const bool& value) const;
    void setParameter(const std::string& name, const int& value) const;
    void setParameter(const std::string& name, const float& value) const;
    void setParameter(const std::string& name, const glm::vec3& value) const;
    void setParameter(const std::string& name, const glm::vec2& value) const;
    void setParameter(const std::string& name, const glm::mat4& value) const;

    [[nodiscard]] uint32_t get_id() const;
};

};  // namespace ck