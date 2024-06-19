#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <assimp/scene.h>

#include "core/ck_debug.h"
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
    /**FIXME - 错题本
    trivially-copyable 平凡可复制对象：本身可以使用memcpy复制内存
    memcpy是内存逐位复制，这个过程本身非常快，不需要使用std::move。
     */
};

struct Texture
{
    uint32_t    id;
    std::string type;
    std::string path;

    Texture() = delete;
    Texture(uint32_t id, std::string type, std::string path)
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

    Mesh(const Mesh&)            = default;
    Mesh& operator=(const Mesh&) = default;
    Mesh(Mesh&&)                 = default;
    Mesh& operator=(Mesh&&)      = default;

    void draw(const Shader& shader) const;

    [[nodiscard]] uint32_t get_vao() const;
    /// @brif 返回第一个最小的可用纹理slot
    [[nodiscard]] int32_t get_avaliable_texture_slot() const;
};

class Model {
private:
    std::vector<Mesh>    meshes;
    std::string          model_directory;
    std::vector<Texture> textures_loaded;

    std::string load_path;

    void                 processNode(const aiNode* node, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(const aiMaterial*  material,
                                              aiTextureType      type,
                                              const std::string& typeName);
    static uint32_t      loadTextureFromFile(const std::string& file_path, bool gamma_correction);

public:
    explicit Model(const std::string& model_path);
    void draw(const Shader& shader) const;

    /// @brief 返回第一个最小的可用纹理slot
    [[nodiscard]] int32_t            get_avaliable_texture_slot() const;
    [[nodiscard]] const std::string& get_load_path() const;

    bool operator==(const Model& other) const;
};

}  // namespace ck

/**FIXME - 错题本
unordered_set 需要Hash作为元素的索引
set 需要排序作为元素的索引
而 Texture 既没有 Hash 算法，也没有排序算法，所以不能直接使用这两个容器
 */
