#include "scene.h"

#include <cstdint>

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>

#include <glm/glm.hpp>
#include <glog/logging.h>
#include <stb_image.h>
#include <string>
#include <utility>

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
    : skyBox_texture(0), pureWhite_skyBox_texture(0), skyBox_color(1.0F),
      skyBox_shader(stdAsset_root + "stdShader/stdSkyboxShader.vs.glsl",
                    stdAsset_root + "stdShader/stdSkyboxShader.fs.glsl"),
      skyBox_model(stdAsset_root + "stdModel/box/box.obj")
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

ck::Scene::Scene()
    : camera(new Camera(glm::vec3(0.0F, 0.5F, -5.0F))), skyBox(new SkyBoxObject()),
      scene_root(new RenderObject(RenderObjectType::NULL_OBJECT, "root"))
{
    objects.push_back(scene_root);  // 创建Scene默认的Root节点
}

void ck::Scene::add_model_prototype(const std::string& model_file_path)
{
    model_prototypes.emplace_back(std::make_shared<Model>(model_file_path));
}

void ck::Scene::add_shader_prototype(const std::array<std::string, 3>& shader_file_path)
{
    shader_prototypes.emplace_back(
        std::make_shared<Shader>(shader_file_path[0], shader_file_path[1], shader_file_path[2]));
}

std::unique_ptr<ck::Scene> ck::Scene::singleton = nullptr;

ck::Scene& ck::Scene::get_instance()
{
    /**NOTE - 静态区变量的初始化时机
    全局变量、类的静态成员变量，在main()函数开始前初始化
    局部静态变量，在第一次使用前初始化

    调用get_instance()时，GLAD已经初始化好了
     */
    // static Scene singleton;
    // return singleton;
    if (singleton == nullptr) { singleton.reset(new Scene()); }
    return *singleton;
    /**FIXME - 错题本
     lld-link: error: undefined symbol: private: static class std::unique_ptr
     静态static但非const对象，要在类class外、命名域namespace内声明。
    */
}

std::shared_ptr<ck::RenderObject>&
ck::Scene::add_model_from_file(const std::string&                model_file_path,
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
    objects.emplace_back(std::make_shared<RenderObject>(RenderObjectType::POLYGEN_MESH, object_name,
                                                        *model_it, Light(-1), *shader_it,
                                                        scene_root.get(), RenderDrawType::NORMAL));
    scene_root->get_children().push_back(objects.back().get());  // 向scene_root添加子节点
    return objects.back();
    /**FIXME - 错题本
    Access violation reading location 0xFFFFFFFFFFFFFFFF.
    std::vector容器是会更改位置的，所以之前拿到的指向容器元素的指针，不能保证未来仍然有效

    * NOTE - 修改方案：
    std::vector<RenderObject>            objects;
    std::vector<std::shared_ptr<RenderObject>>            objects;
    */
}

std::shared_ptr<ck::RenderObject>& ck::Scene::add_light(std::string      object_name,
                                                        const ck::Light& light)
{
    // 在model_prototypes中查找是否有对应的model
    auto model_it = model_prototypes.begin();
    for (; model_it != model_prototypes.end(); model_it++)
    {
        if ((*model_it)->get_load_path() == defualt_light_model_path) { break; }
    }
    if (model_it == model_prototypes.end())  // 如果没有找到，则创建一个新的model
    {
        add_model_prototype(defualt_light_model_path);
        model_it = model_prototypes.end() - 1;  // 新添加的一定在末尾
    }

    // 同理，shader
    auto shader_it = shader_prototypes.begin();
    for (; shader_it != shader_prototypes.end(); shader_it++)
    {
        if ((*shader_it)->get_load_path() == defualt_light_shader_path) { break; }
    }
    if (shader_it == shader_prototypes.end())  // 如果没有找到，则创建一个新的model
    {
        add_shader_prototype(defualt_light_shader_path);
        shader_it = shader_prototypes.end() - 1;  // 新添加的一定在末尾
    }

    objects.emplace_back(std::make_shared<RenderObject>(
        RenderObjectType::LIGHT, std::move(object_name), *model_it, light, *shader_it,
        scene_root.get(), RenderDrawType::NORMAL));
    // 向scene_root添加子节点
    scene_root->get_children().push_back(objects.back().get());
    return objects.back();

    /**FIXME - 问题记录：灯光渲染不出来
    因为render object的model矩阵的计算是根据RenderObject::position计算的，
    而灯光的position写在Light::position中，所以计算的出来的model矩阵是单位矩阵
    灯光被隐藏在黑色的立方体中，自然就看不见了

    * TODO - 修改方案
    Light类中不持有与transform相关的数据，而是由RenderObject持有
     */
}

void ck::Scene::modify_object(const std::shared_ptr<ck::RenderObject>& object_ptr,
                              const ck::SceneObjectEdittingCtx*        ctx)
{
    if (!object_ptr)
    {
        LOG(WARNING) << "object_ptr is nullptr!";
        return;
    }
    if (ctx->object_type == RenderObjectType::NULL_OBJECT)
    {
        LOG(ERROR) << "NULL object or Scene root object cannot be modified!";
        return;
    }

    bool find_matched_object = false;
    bool find_matched_parent = false;
    for (const auto& object : objects)
    {
        if (object_ptr == object) { find_matched_object = true; }
        if (ctx->parent_object != nullptr && ctx->parent_object == object.get())
        {
            find_matched_parent = true;
        }
    }
    if (!find_matched_object)
    {
        LOG(WARNING) << "object_ptr is not a member of objects!";
        return;
    }
    if (ctx->parent_object != nullptr && !find_matched_parent)
    {
        LOG(WARNING) << "parent_object is not a member of objects!";
        return;
    }

    if (ctx->object_type != object_ptr->get_object_type())
    {
        LOG(ERROR) << "object type mismatch!";
        return;
    }

    switch (ctx->object_type)
    {
        case RenderObjectType::POLYGEN_MESH: {
            object_ptr->modify_polygen(ctx);
            break;
        }
        case RenderObjectType::LIGHT: {
            object_ptr->modify_light(ctx);
            break;
        }
        default: break;
    }
}

[[nodiscard]] std::vector<std::shared_ptr<ck::RenderObject>>& ck::Scene::get_scene_objects()
{
    return objects;
}

ck::SkyBoxObject& ck::Scene::get_skyBox()
{
    return *skyBox;
}

ck::Camera& ck::Scene::get_camera()
{
    return *camera;
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
    glClearColor(ctx.skyBox_color[0], ctx.skyBox_color[1], ctx.skyBox_color[2], 1.0F);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    for (const auto& object : objects)
    {
        if (object->get_object_type() != RenderObjectType::NULL_OBJECT) { object->draw(&ctx); }
    }

    skyBox->draw(&ctx);  // 最后渲染天空盒
    GL_CHECK();
}

ck::SceneLightUBOManager::SceneLightUBOManager() : scene(&Scene::get_instance()), lights_UBO(0) {}

void ck::SceneLightUBOManager::create_light_UBO()
{
    // 如果已经存在一个buffer，首先删除它
    if (lights_UBO != 0)
    {
        glDeleteBuffers(1, &lights_UBO);
        lights_UBO = 0;
    }

    glGenBuffers(1, &lights_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);

    // 计算灯光组占据的内存空间（以Byte为单位）
    glBufferData(GL_UNIFORM_BUFFER, calculate_memory_occupation() + 16, nullptr, GL_STATIC_DRAW);
    // NOTE - 这里出GL_STATIC_DRAW，暂且认为灯光的数据初始化后不再修改。
    // FIXME - +16意味着缓冲的前4B是int型的灯光数量，但是要对齐到16B

    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // 解绑

    update_light_UBO();  // 更新灯光组数据}

    GL_CHECK();
}

void ck::SceneLightUBOManager::update_light_UBO() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);
    int stride = Light::calculate_memory_occupancy();
    // 获取指向buffer的指针
    auto* ptr = static_cast<unsigned char*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));

    int32_t num_lights_found = 0;
    for (const auto& object : scene->get_scene_objects())
    {
        if (object->get_object_type() == RenderObjectType::LIGHT)
        {
            // update light UBO
            unsigned char* lightPtr = ptr + num_lights_found * stride + 16;
            // FIXME - +16意味着缓冲的前16B是int型的灯光数量
            const std::array<glm::vec3, 3>& transform = object->get_transform();
            object->get_light().update_light_uniformBuffer(lightPtr, transform[0], transform[1]);

            if ((++num_lights_found) == MAX_LIGHTS_SUPPORTED)
            {
                LOG(WARNING) << "Warning: The number of lights exceeds the maximum supported."
                                "The number of lights will be limited to "
                             << MAX_LIGHTS_SUPPORTED << ".";
                break;
            }
        }
    }

    if (num_lights_found < MAX_LIGHTS_SUPPORTED)
    {
        // 向UBO中写入“空”灯光
        int32_t null_light_type = -1;
        for (int i = num_lights_found; i < MAX_LIGHTS_SUPPORTED; i++)
        {
            // 用“空”灯光更新灯光UBO
            memcpy(ptr + i * stride + 16, &(null_light_type), sizeof(int32_t));
        }
    }

    memcpy(ptr, &(num_lights_found), sizeof(int32_t));  // 更新numLights
    // FIXME - 前16B = 4B整数 + 12B空填充
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // 解绑
    /**FIXME - 有个bug修一下:
    GL_INVALID_OPERATION error generated.
    Target buffer must be bound and not overlapped with mapping range.
    原因是：不可以在解绑glUnmapBuffer()前，继续使用glBufferSubData()
    向同一个target buffer写入数据
    */
}

void ck::SceneLightUBOManager::binding_uniformBuffer(const uint32_t binding_point) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, lights_UBO);
}

void ck::SceneLightUBOManager::print_bufferData() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);
    // 获取指向buffer的指针
    auto* ptr = static_cast<unsigned char*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));

    for (int i = 0; i < 16; i++)
    {
        printf("%02X ", *(ptr + i));
        if ((i + 1) % 4 == 0) { printf(" "); }
        if ((i + 1) % 32 == 0) { printf("\n"); }
    }
    printf("\n");
    ptr += 16;  // FIXME - 前16B放一个int变量

    uint32_t buffer_size = calculate_memory_occupation();

    // 将ptr起始bufferSize大小的数据以16进制的形式打印出来
    for (int i = 0; i < buffer_size; i++)
    {
        printf("%02X ", *(ptr + i));
        if ((i + 1) % 4 == 0) { printf(" "); }
        if ((i + 1) % 32 == 0) { printf("\n"); }
    }

    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // 解绑}
}