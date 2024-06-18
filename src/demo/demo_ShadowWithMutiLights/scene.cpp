#include "scene.h"

#include <glm/glm.hpp>

#include "camera.h"
#include "core/ck_debug.h"
#include "imgui_glfw_window_base.h"
#include "render_object.h"

ck::Scene::Scene() : camera(glm::vec3(0.0F, 0.5F, -5.0F))
{
    // 创建Scene默认的Root节点
    objects.emplace_back(RenderObjectType::NULL_OBJECT, "root");

    /**TODO - 创建一个默认的天空盒
    默认的天空盒式纯白的，配合skyBox_color可以调整颜色。
    用户可以自己添加天空盒，或者使用默认的。
    */
}

ck::Scene::~Scene()
{
    delete singleton;
    singleton = nullptr;
}

ck::Scene& ck::Scene::get_instance()
{
    // 惰性构造
    if (singleton == nullptr) { singleton = new Scene(); }
    return *singleton;
}

void ck::Scene::draw(const ImguiGlfwWindowBase& window) const
{
    // view and projection
    int32_t window_width  = 0;
    int32_t window_height = 0;
    glfwGetFramebufferSize(window.get_window(), &window_width, &window_height);
    const float camera_aspect_ratio =
        static_cast<float>(window_width) / static_cast<float>(window_height);

    RenderingSceneSettingCtx ctx = {};
    ctx.camera                   = &camera;
    ctx.camera_position          = camera.get_position();
    ctx.projection               = camera.get_projection_matrix(camera_aspect_ratio);
    ctx.view                     = camera.get_view_matrix();
    ctx.skyBox_texture           = skyBox_texture;
    ctx.skyBox_color             = skyBox_color;

    // clear
    glClearColor(skyBox_color[0], skyBox_color[1], skyBox_color[2], skyBox_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    for (const auto& object : objects)
    {
        if (object.get_object_type() == RenderObjectType::POLYGEN_MESH ||
            object.get_object_type() == RenderObjectType::LIGHT)
        {
            object.draw(&ctx);
        }
    }
    GL_CHECK();

    // 最后渲染天空盒
    if (enabled_skyBox_index >= 0 && enabled_skyBox_index < objects.size())
    {
        objects[enabled_skyBox_index].draw(&ctx);
    }
    GL_CHECK();
}
