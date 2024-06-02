#pragma once
#include "assimp/scene.h"
#include "ckObject.h"
#include "ckShader.h"
#include "stdInclude.h"

class ckMesh : ckObject {
private:
    std::shared_ptr<ckShader> shader;  // shader可以是共享的

public:
    ckMesh();
    ~ckMesh() = default;

    /// @note Shader挂载在每一个单独的Mesh上
    void drawMesh() const;
};

class ckModel : ckObject {
private:
    std::vector<std::unique_ptr<ckMesh>> meshes;
    std::vector<std::string>             textureLoaded;

    void loadModelFromFile(const char* path);

public:
    /// @brief load model from file
    ckModel(const char* path);
    ~ckModel() = default;

    void drawModel() const;
};
