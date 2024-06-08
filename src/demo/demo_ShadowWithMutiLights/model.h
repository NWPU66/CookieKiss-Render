#pragma once

#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <utility>
#include <vector>

#include <glad/glad.h>  //glad first

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

namespace ck {

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;

    Vertex() = delete;
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoord)
        : position(position), normal(normal), texCoord(texCoord)
    {
    }
};

struct Texture
{
    uint32_t    id;
    std::string type;
    std::string path;

    Texture() = delete;
    Texture(uint32_t id, const std::string type, const std::string path)
        : id(id), type(std::move(type)), path(std::move(path))
    {
    }
};

class Mesh {
private:
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture>  textures;
    uint32_t              vao, vbo, ebo;

    void setup_mesh();

public:
    explicit Mesh(const std::vector<Vertex>&  vertices,
                  const std::vector<GLuint>&  indices,
                  const std::vector<Texture>& textures);

    void draw(const Shader& shader) const;
};

class Model {};

}  // namespace ck