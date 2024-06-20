#include "render_object.h"

#include <cstdint>

#include <algorithm>
#include <memory>
#include <utility>

#include <glm/ext/matrix_transform.hpp>

#include "light.h"
#include "model.h"
#include "shader.h"

void ck::RenderObject::modify_object(const ck::SceneObjectEdittingCtx* ctx)
{
    // 设置共有属性
    // general
    if (!ctx->object_name.empty()) { object_name = ctx->object_name; }
    if (ctx->parent_object != nullptr && object_type != RenderObjectType::NULL_OBJECT)
    {
        // 寻找父级child列表中指向自己的指针
        for (auto it = parent_object->get_children().begin();
             it != parent_object->get_children().end(); it++)
        {
            if (*it == this)
            {
                parent_object->get_children().erase(it);
                break;
            }
        }
        parent_object = ctx->parent_object;
        parent_object->get_children().push_back(this);
    }

    // transformation
    if (ctx->postion != nullptr) { postion = *(ctx->postion); }
    if (ctx->rotation != nullptr) { rotation = *(ctx->rotation); }
    if (ctx->scale != nullptr) { scale = *(ctx->scale); }
}

ck::RenderObject::RenderObject(RenderObjectType               _object_type,
                               std::string                    _object_name,
                               const std::shared_ptr<Model>&  _model,
                               Light                          _light,
                               const std::shared_ptr<Shader>& _shader,
                               RenderObject*                  _parent_object,
                               RenderDrawType                 _draw_type)
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

    // NOTE - 如果是灯光，设置它们的model和shader
    // 现在不设置了，默认创建的时候都有正确设置
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

std::vector<ck::RenderObject*>& ck::RenderObject::get_children()
{
    return children_objects;
}

void ck::RenderObject::modify_polygen(const ck::SceneObjectEdittingCtx* ctx)
{
    if (ctx->model) { model = ctx->model; }
    if (ctx->shader) { shader = ctx->shader; }
    modify_object(ctx);
}

void ck::RenderObject::modify_light(const ck::SceneObjectEdittingCtx* ctx)
{
    if (ctx->light_attributes != nullptr)
    {
        light = Light(ctx->light_attributes->light_type, ctx->light_attributes->color,
                      ctx->light_attributes->intensity, ctx->light_attributes->position,
                      ctx->light_attributes->rotation, ctx->light_attributes->inner_cutOff,
                      ctx->light_attributes->outer_cutOff);
    }
    modify_object(ctx);
}
