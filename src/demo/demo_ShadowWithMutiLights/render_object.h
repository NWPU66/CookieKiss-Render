#pragma once

#include <cstdint>

#include <string>
#include <vector>

#include <glad/glad.h>

#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

#include "camera.h"
#include "light.h"
#include "model.h"

namespace ck {

enum class RenderObjectType : uint32_t { NULL_OBJECT, POLYGEN_MESH, LIGHT, SKYBOX };

struct RenderingSceneSettingCtx
{
    glm::mat4     view;
    glm::mat4     projection;
    const Camera* camera;
    glm::vec3     camera_position;
    uint32_t      skyBox_texture;
    glm::vec3     skyBox_color;
};

/// @brief 只有当对象是多边形几何体时，RenderDrawType才有意义
enum class RenderDrawType : uint32_t {
    NORMAL     = 1 << 0,
    UNVISIABLE = 1 << 1,
    OUTLINE    = 1 << 2,
    SHADOW     = 1 << 3,
    FLASHING   = 1 << 4,
    BLUR       = 1 << 5
};

/// @brief 渲染对象可以是几何体、灯光。。。
/// @note 灯光没设计好，以后还是拆开单独设计好了
/// 平时的时候单独存储，烘培的时候打组统一更新uniform buffer
class RenderObject {
private:
    RenderObjectType           object_type;
    std::string                object_name;
    RenderObject*              parent_object;
    std::vector<RenderObject*> children_objects;

    Model*  model;  // 如果是灯光，则使用灯光默认的模型（sphere）
    Light   light;
    Shader* shader;

    // transformation
    glm::vec3 postion;
    glm::vec3 rotation;
    glm::vec3 scale;

    RenderDrawType draw_type;

public:
    explicit RenderObject(RenderObjectType _object_type,
                          std::string      _object_name,
                          Model*           _model         = nullptr,
                          Light            _light         = Light(-1),
                          Shader*          _shader        = nullptr,
                          RenderObject*    _parent_object = nullptr,
                          RenderDrawType   _draw_type     = RenderDrawType::NORMAL);
    void                           draw(const RenderingSceneSettingCtx* ctx) const;
    [[nodiscard]] RenderObjectType get_object_type() const;
};

};  // namespace ck