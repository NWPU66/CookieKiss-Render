#include "scene.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>

#include <glm/glm.hpp>
#include <glog/logging.h>
#include <stb_image.h>

#include "camera.h"
#include "core/ck_debug.h"
#include "imgui_glfw_window_base.h"
#include "light.h"
#include "model.h"
#include "render_object.h"
#include "shader.h"

extern const std::string stdAsset_root;
/**FIXME - 错题本：多个文件中共享const对象
默认情况下，一个const对象仅在本文件内有效，如果多个文件中出现了
同名的const变量时，其实等同于在不同的文件中分别定义了独立的变量。

在某些情况下，我们不希望编译器为每个文件分别生成独立的变量。
也就是说，只在一个文件中定义const，而在其他多个文件中声明并使用它。

方法是对于const变量不管是声明还是定义都添加extern关键字，
这样只需要定义一次就可以了：
*/

ck::SkyBoxObject::SkyBoxObject()
    : skyBox_texture(0), pureWhite_skyBox_texture(0), skyBox_color(1),
      skyBox_shader(stdAsset_root + "stdShader/stdSkyboxShader.vs.glsl",
                    stdAsset_root + "stdShader/stdSkyboxShader.fs.glsl"),
      skyBox_model(stdAsset_root + "stdModel/box.obj")
{
    /**NOTE - 创建一个默认的天空盒
    默认的天空盒式纯白的，配合skyBox_color可以调整颜色。
    用户可以自己添加天空盒，或者使用默认的。
    */
    create_skyBox_texture_from_file(pureWhite_skyBox_texture, stdAsset_root + "stdTexture/skybox/");
}

ck::SkyBoxObject::~SkyBoxObject()
{
    glDeleteTextures(1, &pureWhite_skyBox_texture);
    if (skyBox_texture != 0) { glDeleteTextures(1, &skyBox_texture); }
}

[[nodiscard]] uint32_t ck::SkyBoxObject::get_skyBox_texture() const
{
    if (skyBox_texture == 0) { return pureWhite_skyBox_texture; }
    return skyBox_texture;
}

[[nodiscard]] glm::vec3 ck::SkyBoxObject::get_skyBox_color() const
{
    return skyBox_color;
}

void ck::SkyBoxObject::create_skyBox_texture_from_file(uint32_t&          target_texture,
                                                       const std::string& image_folder)
{
    glGenTextures(1, &target_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, target_texture);
    std::vector<std::string> cubeTexture_names = {"right.jpg",  "left.jpg",  "top.jpg",
                                                  "bottom.jpg", "front.jpg", "back.jpg"};
    for (int i = 0; i < 6; i++)
    {
        std::string cubeTexture_path = image_folder + cubeTexture_names[i];
        int         width            = 0;
        int         height           = 0;
        int         nrChannels       = 0;
        stbi_set_flip_vertically_on_load(0);  // 加载图片时翻转y轴
        GLubyte* data = stbi_load(cubeTexture_path.c_str(), &width, &height, &nrChannels, 0);
        if (data != nullptr)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB,
                         GL_UNSIGNED_BYTE, data);
            // 设置纹理属性
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        else { LOG(WARNING) << "Failed to load texture: " << cubeTexture_path; }
        stbi_image_free(data);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);  // 解绑
}

void ck::SkyBoxObject::load_skyBox_texture_from_file(const std::string& image_folder)
{
    if (skyBox_texture != 0) { glDeleteTextures(1, &skyBox_texture); }
    create_skyBox_texture_from_file(skyBox_texture, image_folder);
}

void ck::SkyBoxObject::draw(const RenderingSceneSettingCtx* ctx) const
{
    glFrontFace(GL_CW);  // 把顺时针的面设置为“正面”。
    skyBox_shader.use();
    skyBox_shader.setParameter("view",
                               glm::mat4(glm::mat3(ctx->view)));  // 除去位移，相当于锁头
    skyBox_shader.setParameter("projection", ctx->projection);

    // sky box texture
    int32_t skyBox_texture_slot = skyBox_model.get_avaliable_texture_slot();
    glActiveTexture(GL_TEXTURE0 + skyBox_texture_slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox_texture);
    skyBox_shader.setParameter("skybox", skyBox_texture_slot);

    skyBox_model.draw(skyBox_shader);
    glFrontFace(GL_CCW);
    GL_CHECK();
}

ck::Scene::Scene() : camera(new Camera(glm::vec3(0.0F, 0.5F, -5.0F))), skyBox(new SkyBoxObject())
{
    // 创建Scene默认的Root节点
    objects.emplace_back(RenderObjectType::NULL_OBJECT, "root");
    scene_root.reset(objects.data());
}

void ck::Scene::add_model_prototype(const std::string& model_file_path)
{
    std::shared_ptr<Model> new_model(new Model(model_file_path));
    model_prototypes.push_back(std::move(new_model));
}

void ck::Scene::add_shader_prototype(const std::array<std::string, 3>& shader_file_path)
{
    std::shared_ptr<Shader> new_shader(
        new Shader(shader_file_path[0], shader_file_path[1], shader_file_path[2]));
    shader_prototypes.push_back(std::move(new_shader));
}

ck::Scene& ck::Scene::get_instance()
{
    /**NOTE - 静态区变量的初始化时机
    全局变量、类的静态成员变量，在main()函数开始前初始化
    局部静态变量，在第一次使用前初始化

    调用get_instance()时，GLAD已经初始化好了
     */
    static Scene singleton;
    return singleton;
}

void ck::Scene::add_model_from_file(const std::string&                model_file_path,
                                    const std::array<std::string, 3>& shader_file_path,
                                    const std::string&                object_name)
{
    // 在model_prototypes中查找是否有对应的model
    auto model_it = model_prototypes.begin();
    for (; model_it != model_prototypes.end(); model_it++)
    {
        if ((*model_it)->get_load_path() == model_file_path) { break; }
    }
    if (model_it == model_prototypes.end())
    {
        // 如果没有找到，则创建一个新的model
        add_model_prototype(model_file_path);
        model_it = model_prototypes.end() - 1;  // 新添加的一定在末尾
    }

    // 同理，shader
    auto shader_it = shader_prototypes.begin();
    for (; shader_it != shader_prototypes.end(); shader_it++)
    {
        if ((*shader_it)->get_load_path() == shader_file_path) { break; }
    }
    if (shader_it == shader_prototypes.end())
    {
        // 如果没有找到，则创建一个新的model
        add_shader_prototype(shader_file_path);
        shader_it = shader_prototypes.end() - 1;  // 新添加的一定在末尾
    }

    // 创建object
    objects.emplace_back(RenderObjectType::POLYGEN_MESH, object_name, *model_it, Light(-1),
                         *shader_it, scene_root.get(), RenderDrawType::NORMAL);
    scene_root->get_children().push_back(&objects.back());  // 向scene_root添加子节点
}

void ck::Scene::modify_object(int32_t object_index, const ck::SceneObjectEdittingCtx* ctx)
{
    if (object_index < 0 || object_index >= objects.size())
    {
        LOG(ERROR) << "cannot modify object whose index is out of the range!";
        return;
    }
    if (*(ctx->object_type) == RenderObjectType::NULL_OBJECT)
    {
        LOG(ERROR) << "NULL object or Scene root object cannot be modified!";
        return;
    }

    switch (*(ctx->object_type))
    {
        case RenderObjectType::POLYGEN_MESH: {
            objects[object_index].modify_polygen(ctx);
            break;
        }
        case RenderObjectType::LIGHT: {
            objects[object_index].modify_light(ctx);
            break;
        }
        default: break;
    }
}

[[nodiscard]] std::vector<ck::RenderObject>& ck::Scene::get_scene_objects()
{
    return objects;
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
    ctx.camera                   = camera.get();
    ctx.camera_position          = camera->get_position();
    ctx.projection               = camera->get_projection_matrix(camera_aspect_ratio);
    ctx.view                     = camera->get_view_matrix();
    ctx.skyBox_texture           = skyBox->get_skyBox_texture();
    ctx.skyBox_color             = skyBox->get_skyBox_color();

    // clear
    glClearColor(ctx.skyBox_color[0], ctx.skyBox_color[1], ctx.skyBox_color[2],
                 ctx.skyBox_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    for (const auto& object : objects)
    {
        object.draw(&ctx);
    }

    skyBox->draw(&ctx);  // 最后渲染天空盒
    GL_CHECK();
}
