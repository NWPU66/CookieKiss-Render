#include "render_object.h"

#include <cstdint>

#include <utility>

#include <glm/ext/matrix_transform.hpp>

#include "shader.h"

ck::RenderObject::RenderObject(RenderObjectType _object_type,
                               std::string      _object_name,
                               Model*           _model,
                               Light            _light,
                               Shader*          _shader,
                               RenderObject*    _parent_object,
                               RenderDrawType   _draw_type)
    : object_type(_object_type), model(_model), light(_light), shader(_shader),
      object_name(std::move(_object_name)), parent_object(_parent_object), postion(0), rotation(0),
      scale(1), draw_type(_draw_type)
{
    // NOTE - 允许用RenderObjectType::NULL_OBJECT来创建Scene的Root节点
    // if (_object_type == RenderObjectType::NULL_OBJECT)
    // {
    //     LOG(ERROR) << "null object type to create a new render object!";
    // }
    // if (_object_name.empty()) { LOG(ERROR) << "no object name given!"; }

    // TODO - 如果是灯光或天空盒，设置它们的model和shader
    switch (object_type)
    {
        case RenderObjectType::LIGHT: {
        }
        case RenderObjectType::SKYBOX: {
        }
    }
}

void ck::RenderObject::draw(const RenderingSceneSettingCtx* ctx) const
{
    if (shader == nullptr || model == nullptr)
    {
        LOG(ERROR) << "no model or shader given to render";
        return;
    }

    // transform martix
    glm::mat4 matrix_model = glm::translate(glm::mat4(1), postion);
    matrix_model           = glm::rotate(matrix_model, rotation.x, glm::vec3(1, 0, 0));
    matrix_model           = glm::rotate(matrix_model, rotation.y, glm::vec3(0, 1, 0));
    matrix_model           = glm::rotate(matrix_model, rotation.z, glm::vec3(0, 0, 1));
    matrix_model           = glm::scale(matrix_model, scale);

    switch (object_type)
    {
        case RenderObjectType::POLYGEN_MESH: {
            shader->use();
            shader->setParameter("model", matrix_model);
            shader->setParameter("view", ctx->view);
            shader->setParameter("projection", ctx->projection);
            shader->setParameter("cameraPos", ctx->camera_position);

            // sky box texture
            int32_t skyBox_texture_slot = model->get_avaliable_texture_slot();
            glActiveTexture(GL_TEXTURE0 + skyBox_texture_slot);
            glBindTexture(GL_TEXTURE_CUBE_MAP, ctx->skyBox_texture);
            shader->setParameter("skybox", skyBox_texture_slot);

            model->draw(*shader);
            GL_CHECK();
            break;

            // TODO - 根据RenderDrawType实现不同的渲染效果
        }
        case RenderObjectType::LIGHT: {
            shader->use();
            shader->setParameter("model", matrix_model);
            shader->setParameter("view", ctx->view);
            shader->setParameter("projection", ctx->projection);
            shader->setParameter("lightColor", light.get_color());
            model->draw(*shader);
            GL_CHECK();
            break;
        }
        case RenderObjectType::SKYBOX: {
            glFrontFace(GL_CW);  // 把顺时针的面设置为“正面”。
            shader->use();
            shader->setParameter("view",
                                 glm::mat4(glm::mat3(ctx->view)));  // 除去位移，相当于锁头
            shader->setParameter("projection", ctx->projection);

            // sky box texture
            int32_t skyBox_texture_slot = model->get_avaliable_texture_slot();
            glActiveTexture(GL_TEXTURE0 + skyBox_texture_slot);
            glBindTexture(GL_TEXTURE_CUBE_MAP, ctx->skyBox_texture);
            shader->setParameter("skybox", skyBox_texture_slot);

            model->draw(*shader);
            glFrontFace(GL_CCW);
            GL_CHECK();
            break;
        }
        default: {
            LOG(ERROR) << "unknown object type to draw!";
            return;
        }
    }
}

[[nodiscard]] ck::RenderObjectType ck::RenderObject::get_object_type() const
{
    return object_type;
}
