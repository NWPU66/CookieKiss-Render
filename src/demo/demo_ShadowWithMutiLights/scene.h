#pragma once


#include <cstdint>

#include <memory>
#include <string>
#include <vector>
#include <array>

#include <glad/glad.h>

#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

#include "camera.h"
#include "imgui_glfw_window_base.h"
#include "light.h"
#include "model.h"
#include "render_object.h"
#include "shader.h"

namespace ck {

class SkyBoxObject {
private:
    uint32_t  skyBox_texture;
    uint32_t  pureWhite_skyBox_texture;
    glm::vec3 skyBox_color;
    Shader    skyBox_shader;
    Model     skyBox_model;

    static void create_skyBox_texture_from_file(uint32_t&          target_texture,
                                                const std::string& image_folder);

public:
    SkyBoxObject();
    ~SkyBoxObject();

    [[nodiscard]] uint32_t  get_skyBox_texture() const;
    [[nodiscard]] glm::vec3 get_skyBox_color() const;

    void load_skyBox_texture_from_file(const std::string& image_folder);
    void draw(const RenderingSceneSettingCtx* ctx) const;
};

/// @brief 场景类
/// @note 设计成单例类
class Scene {
private:
    /**NOTE - object in the scene
    允许多个object的model/shader，指向同一个model_prototypes/shader_prototypes
    即同一个model_prototypes/shader_prototypes实例出多个model/shader

    当引用计数为1的时候表示该model/shader不再被引用
    */
    std::vector<std::shared_ptr<Model>>  model_prototypes;
    std::vector<std::shared_ptr<Shader>> shader_prototypes;
    std::vector<RenderObject>            objects;
    std::unique_ptr<RenderObject>        scene_root;

    std::unique_ptr<Camera>       camera;
    std::unique_ptr<SkyBoxObject> skyBox;

    // TODO - shadowMap baking system

    /**NOTE - singleton class
    定义移动构造器（删除也算进行了定义），
    且没有定义复制构造器、复制赋值、移动赋值时，
    上述四个函数将全部无法使用，由此构成单例类。
    */
    Scene();
    Scene(Scene&&) = delete;

    void add_model_prototype(const std::string& model_file_path);
    void add_shader_prototype(const std::array<std::string, 3>& shader_file_path);

public:
    static Scene& get_instance();

    /// @brief 添加一个模型到场景中
    /// @note 从已加载的prototypes中快速加载，由内部判断和实现
    void add_model_from_file(const std::string&                model_file_path,
                             const std::array<std::string, 3>& shader_file_path,
                             const std::string&                object_name);

    /// @brief 添加一个灯光到场景中
    /// @note 灯光有默认的mesh和shader
    void add_light();

    void modify_object(int32_t object_index, const SceneObjectEdittingCtx* ctx);

    [[nodiscard]] std::vector<RenderObject>& get_scene_objects();

    void draw(const ImguiGlfwWindowBase& window) const;
};
/**FIXME - Call to implicitly-deleted default constructor
类成员变量会在构造函数“函数体”前进行初始化
如果没有显式指定如何构造它们，则会调用默认的构造函数
其中Shader没有默认的构造函数
*/

}  // namespace ck