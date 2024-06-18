#pragma once

#include <cstdint>

#include <string>
#include <vector>

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

/// @brief 场景类
/// @note 设计成单例类
class Scene {
private:
    /**NOTE - object in the scene
    允许多个object的model/shader，指向同一个model_prototypes/shader_prototypes
    即同一个model_prototypes/shader_prototypes实例出多个model/shader
    */
    std::vector<Model>        model_prototypes;
    std::vector<Shader>       shader_prototypes;
    std::vector<RenderObject> objects;
    Camera                    camera;

    // skyBox
    int32_t   enabled_skyBox_index;
    uint32_t  skyBox_texture;
    glm::vec3 skyBox_color;

    // TODO - shadowMap baking system

    /**NOTE - singleton class
    定义移动构造器（删除也算进行了定义），
    且没有定义复制构造器、复制赋值、移动赋值时，
    上述四个函数将全部无法使用，由此构成单例类。
    */
    static inline Scene* singleton = nullptr;
    Scene();
    ~Scene();
    Scene(Scene&&) = delete;

public:
    static Scene& get_instance();

    void draw(const ImguiGlfwWindowBase& window) const;
};
/**FIXME - Call to implicitly-deleted default constructor
类成员变量会在构造函数“函数体”前进行初始化
如果没有显式指定如何构造它们，则会调用默认的构造函数
其中Shader没有默认的构造函数
*/

class SceneManager {
private:
public:
    static void add_model_from_file();
    static void add_light();
    static void bake_shadowMap();
};

}  // namespace ck