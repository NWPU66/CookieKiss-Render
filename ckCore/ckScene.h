#pragma once
#include "stdInclude.h"
#include "ckObject.h"
#include "ckCamera.h"
#include "ckLight.h"
#include "ckModel.h"

class ckScene : ckObject
{
private:
    std::vector<std::unique_ptr<ckCamera>> cameraGroup;
    std::vector<std::unique_ptr<ckModel>>  modelGroup;
    std::vector<std::unique_ptr<ckLight>>  lightGroup;
    int32_t                                activeCamera = -1; //-1 means no active camera

public:
    ckScene();
    ~ckScene() = default;

    //get method
    const std::vector<std::unique_ptr<ckCamera>>& getCameraGroup() const;
    const std::vector<std::unique_ptr<ckModel>>&  getModelGroup() const;
    const std::vector<std::unique_ptr<ckLight>>&  getLightGroup() const;
    int32_t                                       getActiveCamera() const;

    //set method
    void setActiveCamera(const int32_t cameraIdx);

    /// @brief 递归绘制整个场景
    /// @note Shader挂载在每一个单独的Mesh上
    void drawScene() const;
};
