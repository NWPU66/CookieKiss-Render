#pragma once

#include <cstdint>
#include <cstdlib>

#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <glad/glad.h>  //glad first

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "glog/logging.h"
#include "stb_image.h"
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
    std::vector<Texture> textures;
    int32_t              indices_num;
    uint32_t             vao, vbo, ebo;

public:
    Mesh(const aiMesh* mesh, std::vector<Texture>& textures);
    ~Mesh();

    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&)                 = delete;
    Mesh& operator=(Mesh&&)      = delete;

    void                   draw(const Shader& shader) const;
    [[nodiscard]] uint32_t get_vao() const;
};

class Model {
private:
    std::vector<Mesh>           meshes;
    std::string                 model_directory;
    std::unordered_set<Texture> textures_loaded;

    void                 processNode(const aiNode* node, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(const aiMaterial*  material,
                                              aiTextureType      type,
                                              const std::string& typeName);
    static uint32_t      loadTextureFromFile(const std::string& file_path, bool gamma_correction);

public:
    explicit Model(const std::string& model_path);
    void draw(const Shader& shader) const;
};

}  // namespace ck