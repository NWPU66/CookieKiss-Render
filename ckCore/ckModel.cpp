#include "ckModel.h"

void ckMesh::drawMesh() const
{
    //检查有没有挂载Shader
    if (!shader)
    {
        debugLog(ckDebugLevel::HINT,
                 "No shader attached to mesh, use default shader instead!");
        // TODO 创建一个默认的shader
    }

    //TODO 渲染网格
}

ckModel::ckModel(const char* path)
{
    debugLog();
    loadModelFromFile(path);
}

void ckModel::loadModelFromFile(const char* path) {}
