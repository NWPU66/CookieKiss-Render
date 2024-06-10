#pragma once

#include <cstdint>

#include <vector>

#include <glm/glm.hpp>

static const uint32_t MAX_LIGHTS_SUPPORTED = 16;

namespace ck {

/// @param lightType -1代表无效灯，0点光，1日光，2聚光，3面光。
class Light {
private:
    int32_t   light_type;
    glm::vec3 color;
    float     intensity;
    glm::vec3 position, rotation;
    float     inner_cutOff, outer_cutOff;  // for spot light

public:
    explicit Light(int32_t   light_type   = -1,
                   glm::vec3 color        = glm::vec3(1),
                   float     intensity    = 1,
                   glm::vec3 position     = glm::vec3(0),
                   glm::vec3 rotation     = glm::vec3(0),
                   float     inner_cutOff = cos(glm::radians(12.5F)),
                   float     outer_cutOff = cos(glm::radians(17.5F)));

    [[nodiscard]] glm::vec3 get_postion() const;
    [[nodiscard]] glm::vec3 get_rotation() const;
    [[nodiscard]] int32_t   get_light_type() const;
    [[nodiscard]] glm::vec3 get_color() const;

    /// @brief 计算在OpenGL 140布局下，单个灯光在缓冲中占用的空间
    /// @return 占用的缓冲空间，以Byte为单位
    constexpr static uint32_t calculate_memory_occupancy();

    /// @brief 在以ptr为起始地址的空间上更新灯光的数据
    /// @param ptr 灯光Uniform缓冲的起始地址
    void update_light_uniformBuffer(unsigned char* ptr) const;
};

/// @brief 灯光组
/// @note 使用方法：
/// 1-addLight()
/// 2-updateLightUniformBuffer()
/// 3-bindingUniformBuffer()
/// 4-正常使用
/// 5-（可选）更新灯光数据
/// 6-updateLightUniformBuffer()
class LightGroup {
private:
    std::vector<Light> lights;
    uint32_t           lights_UBO;

public:
    LightGroup();

    [[nodiscard]] const Light&              get_light(int32_t index) const;
    [[nodiscard]] const std::vector<Light>& get_lights() const;

    void add_light(const Light& light);
    void add_light(const std::vector<Light>& _lights);
    void remove_light(int32_t index);

    /// @brief 创建灯光组的Uniform缓冲
    void create_light_uniformBuffer();

    /// @brief 更新灯光组的Uniform缓冲
    void update_light_uniformBuffer() const;

    /// @brief 将灯光组的Uniform缓冲绑定到系统的绑定点上
    /// @param binding_point 绑定点
    void binding_uniformBuffer(uint32_t binding_point) const;

    /// @brief 将灯光组Uniform缓冲的一部分绑定到系统的绑定点上
    /// @param binding_point 绑定点
    /// @param offset 起始偏移
    /// @param size 预绑定的数据大小
    void binding_uniformBuffer(uint32_t binding_point, uint32_t offset, uint32_t size) const;

    /// @brief 计算在OpenGL 140布局下，整个灯光组在缓冲中占用的空间
    /// @return 占用的缓冲空间，以Byte为单位
    [[nodiscard]] uint32_t calculate_memory_occupation() const;

    void print_bufferData() const;
};

};  // namespace ck