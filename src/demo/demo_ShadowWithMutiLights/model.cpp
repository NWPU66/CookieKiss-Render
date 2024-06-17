#include "model.h"

#include <GL/gl.h>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stb_image.h>

#include "shader.h"

ck::Mesh::Mesh(const aiMesh* mesh, std::vector<Texture>& textures) : textures(std::move(textures))
{
    // 申请缓冲
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // 预计算texCoords和indices，并计算缓冲区的总大小
    // texCoords
    std::vector<glm::vec2> mesh_texCoords;
    if (mesh->HasTextureCoords(0))
    {
        for (int i = 0; i < mesh->mNumVertices; i++)
        {
            mesh_texCoords.emplace_back(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            /**FIXME - emplace_back 和 push_back 的区别：
            push_back先用普通构造创建一个临时对象，再触发移动构造，最后将临时对象移动到容器中。
            emplace_back直接用参数在容器末尾构造对象构造对象。
            */
        }
    }
    else { mesh_texCoords.resize(mesh->mNumVertices, glm::vec2(0.0F, 0.0F)); }
    // indices
    std::vector<uint32_t> indices;
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    indices_num = indices.size();

    // 计算缓冲区的总大小
    GLsizeiptr total_vertex_buffer_size =
        mesh->mNumVertices * sizeof(aiVector3D)       // 顶点位置
        + mesh->mNumVertices * sizeof(aiVector3D)     // 顶点法向
        + mesh_texCoords.size() * sizeof(glm::vec2);  // 顶点纹理坐标
    GLsizeiptr total_element_buffer_size = indices_num * sizeof(uint32_t);
    // 申请
    glBufferData(GL_ARRAY_BUFFER, total_vertex_buffer_size, nullptr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, total_element_buffer_size, nullptr, GL_STATIC_DRAW);
    // 向缓冲区填充顶点数据
    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh->mNumVertices * sizeof(aiVector3D), mesh->mVertices);
    glBufferSubData(GL_ARRAY_BUFFER, mesh->mNumVertices * sizeof(aiVector3D),
                    mesh->mNumVertices * sizeof(aiVector3D), mesh->mNormals);
    glBufferSubData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(aiVector3D),
                    mesh_texCoords.size() * sizeof(glm::vec2), mesh_texCoords.data());
    // 向缓冲区填充数据
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_num * sizeof(uint32_t), indices.data(),
                 GL_STATIC_DRAW);
    /**FIXME - 错误定位：//生成顶点数据（26~46）
    在使用glBufferSubData填充数据之前，要先申请一片缓冲区（glBufferData）。
    */

    // 设置VAO中数据的解读方式
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
                          (void*)(mesh->mNumVertices * sizeof(aiVector3D)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0,
                          (void*)(2 * mesh->mNumVertices * sizeof(aiVector3D)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // 解绑
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GL_CHECK();
}

ck::Mesh::~Mesh()
{
    // opengl释放缓存区
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void ck::Mesh::draw(const Shader& shader) const
{
    // shader.use();

    // 设置贴图纹理
    uint32_t diffuseNr  = 0;
    uint32_t specularNr = 0;
    uint32_t normalNr   = 0;
    for (int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);  // 在绑定纹理之前激活相应的纹理单元
        glBindTexture(GL_TEXTURE_2D,
                      textures[i].id);  // FIXME - 启动槽之后记得要绑定纹理啊

        std::string name = textures[i].type;
        if (name == "texture_diffuse") { name += std::to_string(diffuseNr++); }
        else if (name == "texture_specular") { name += std::to_string(specularNr++); }
        else if (name == "texture_normal") { name += std::to_string(normalNr++); }

        shader.setParameter(name, i);
    }

    // 绘制
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices_num, GL_UNSIGNED_INT, 0);
    // glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceNum);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults
    glBindTexture(GL_TEXTURE_2D, 0);

    GL_CHECK();
}

[[nodiscard]] uint32_t ck::Mesh::get_vao() const
{
    return vao;  // NOTE - 返回的是int的副本，确实没必要出const
}

ck::Model::Model(const std::string& model_path)
{
    std::cout << "model_path: " << model_path << std::endl;
    if (model_path.empty())
    {
        LOG(WARNING) << "model path is empty, please check your model path";
        return;
    }

    // load model from path
    Assimp::Importer importer;
    const aiScene*   scene =
        importer.ReadFile(model_path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (scene == nullptr || ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U) ||
        scene->mRootNode == nullptr)
    {
        LOG(ERROR) << "ERROR::ASSIMP::" << importer.GetErrorString();
        return;
    }

    model_directory = model_path.substr(0, model_path.find_last_of('/'));

    // 层序遍历
    std::queue<const aiNode*> node_queue;
    node_queue.push(scene->mRootNode);
    while (!node_queue.empty())
    {
        // process node in the front
        const aiNode* node = node_queue.front();
        processNode(node, scene);

        // add child nodes to queue
        for (int i = 0; i < node->mNumChildren; ++i)
        {
            node_queue.push(node->mChildren[i]);
        }
        node_queue.pop();
    }

    // NOTE - 内存的释放由Assimp::Importer importer对象的析构自动完成
    GL_CHECK();
}

void ck::Model::processNode(const aiNode* node, const aiScene* scene)
{
    // 处理节点所有的网格
    for (int i = 0; i < node->mNumMeshes; ++i)
    {
        // 处理材质纹理
        std::vector<Texture> textures;
        uint32_t             material_index = scene->mMeshes[node->mMeshes[i]]->mMaterialIndex;
        if (material_index >= 0)
        {
            aiMaterial* material = scene->mMaterials[material_index];

            // diffuse
            std::vector<Texture> diffuseMaps =
                loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            std::cout << "num of diffuse texture: " << diffuseMaps.size() << std::endl;

            // specular
            std::vector<Texture> specularMaps =
                loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // normal
            std::vector<Texture> normalMaps =
                loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // FIXME - 为什么法线贴图的类型是 aiTextureType_HEIGHT ？
        }

        meshes.emplace_back(scene->mMeshes[node->mMeshes[i]], textures);
    }
    GL_CHECK();
}

std::vector<ck::Texture> ck::Model::loadMaterialTextures(const aiMaterial*   material,
                                                         const aiTextureType type,
                                                         const std::string&  typeName)
{
    std::vector<Texture> textures;
    for (int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString texture_path;
        material->GetTexture(type, i, &texture_path);

        bool skip = false;
        for (const auto& texture_loaded : textures_loaded)
        {
            if (texture_loaded.path == std::string(texture_path.C_Str()))
            {
                skip = true;
                textures.push_back(texture_loaded);
                break;
            }
        }

        // load texture from path
        if (!skip)
        {
            uint32_t texture_id =
                loadTextureFromFile(model_directory + '/' + std::string(texture_path.C_Str()),
                                    (type == aiTextureType_DIFFUSE));
            textures.emplace_back(texture_id, typeName, std::string(texture_path.C_Str()));
            textures_loaded.emplace_back(texture_id, typeName, std::string(texture_path.C_Str()));
        }
    }
    GL_CHECK();
    return textures;
}

uint32_t ck::Model::loadTextureFromFile(const std::string& file_path, const bool gamma_correction)
{
    LOG(INFO) << "load texture from file: " << file_path;

    // 生成并绑定纹理对象
    GLuint texture_id = 0;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // 读取图片
    int            width        = 0;
    int            height       = 0;
    int            nrComponents = 0;
    unsigned char* data         = stbi_load(file_path.c_str(), &width, &height, &nrComponents, 0);
    if (data != nullptr)
    {
        // 设置纹理内部格式
        int32_t internal_format = 0;
        GLenum  format          = 0;
        if (nrComponents == 1)
        {
            internal_format = GL_RED;
            format          = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
            if (gamma_correction) { internal_format = GL_SRGB; }
            else { internal_format = GL_RGB; }
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
            if (gamma_correction) { internal_format = GL_SRGB_ALPHA; }
            else { internal_format = GL_RGBA; }
        }
        else { LOG(WARNING) << "no sutibale format for texture: " << file_path; }

        // 创建纹理对象的数据
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE,
                     data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 纹理设置
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        LOG(WARNING) << "load texture failed: " << file_path;
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);  // 释放图片的内存
    GL_CHECK();
    return texture_id;
}

void ck::Model::draw(const Shader& shader) const
{
    shader.use();
    GLenum last_active_texture;
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
    for (const auto& mesh : meshes)
    {
        mesh.draw(shader);
    }
    GL_CHECK();
}
