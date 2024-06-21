#include "light.h"

#include <cstdint>

#include <iostream>

#include <glad/glad.h>  //glad first

#include <glog/logging.h>

#include "core/ck_debug.h"

ck::Light::Light(const int32_t   light_type,
                 const glm::vec3 color,
                 const float     intensity,
                 const glm::vec3 position,
                 const glm::vec3 rotation,
                 const float     inner_cutOff,
                 const float     outer_cutOff)
    : light_type(light_type), color(color), intensity(intensity), position(position),
      rotation(rotation), inner_cutOff(inner_cutOff), outer_cutOff(outer_cutOff)
{
}

[[nodiscard]] glm::vec3 ck::Light::get_postion() const
{
    return position;
}

[[nodiscard]] glm::vec3 ck::Light::get_rotation() const
{
    return rotation;
}

[[nodiscard]] int32_t ck::Light::get_light_type() const
{
    return light_type;
}

[[nodiscard]] glm::vec3 ck::Light::get_color() const
{
    return color;
}

constexpr uint32_t ck::Light::calculate_memory_occupancy()
{
    /**NOTE - memory occupation
     * GLint     lightType;（4B）
     * glm::vec3 position;（16B）
     * GLfloat   intensity;（4B）
     * glm::vec3 position, rotation;（16B，16B）
     * GLfloat innerCutOff, outerCutOf,;（4B，4B）
     */
    /**FIXME - 错题本
     * std140的布局理解错了！
     * 详细的计算分析请看 opengl/src/advancedLighting/memoryLayout.md
     */
    return 80;
}

void ck::Light::update_light_uniformBuffer(unsigned char* ptr) const
{
    memcpy(ptr + 0, &(light_type), sizeof(int32_t));
    memcpy(ptr + 16, &(color), sizeof(glm::vec3));
    memcpy(ptr + 28, &(intensity), sizeof(float));
    memcpy(ptr + 32, &(position), sizeof(glm::vec3));
    memcpy(ptr + 48, &(rotation), sizeof(glm::vec3));
    memcpy(ptr + 60, &(inner_cutOff), sizeof(float));
    memcpy(ptr + 64, &(outer_cutOff), sizeof(float));
    /**FIXME - 错题本
     * std140的布局理解错了！
     * 详细的计算分析请看 opengl/src/advancedLighting/memoryLayout.md
     */
}

ck::LightGroup::LightGroup() : lights(0), lights_UBO(0) {}

[[nodiscard]] const ck::Light& ck::LightGroup::get_light(int32_t index) const
{
    return lights[index];
}

[[nodiscard]] const std::vector<ck::Light>& ck::LightGroup::get_lights() const
{
    return lights;
}

void ck::LightGroup::add_light(const Light& light)
{
    if (lights.size() < MAX_LIGHTS_SUPPORTED) { lights.push_back(light); }
    else { LOG(ERROR) << "light number exceeds the maximum supported number!"; }
}

void ck::LightGroup::add_light(const std::vector<Light>& _lights)
{
    if (lights.size() + _lights.size() <= MAX_LIGHTS_SUPPORTED)
    {
        lights.insert(lights.end(), _lights.begin(), _lights.end());
    }
    else { LOG(ERROR) << "light number exceeds the maximum supported number!"; }
}

void ck::LightGroup::remove_light(int32_t index)
{
    if (index >= 0 && index < lights.size()) { lights.erase(lights.begin() + index); }
    else { LOG(ERROR) << "light index is out of range!"; }
}

void ck::LightGroup::create_light_uniformBuffer()
{
    glGenBuffers(1, &lights_UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);

    // 计算灯光组占据的内存空间（以Byte为单位）
    glBufferData(GL_UNIFORM_BUFFER, calculate_memory_occupation() + 16, nullptr, GL_STATIC_DRAW);
    // NOTE - 这里出GL_STATIC_DRAW，暂且认为灯光的数据初始化后不再修改。
    // FIXME - +16意味着缓冲的前4B是int型的灯光数量，但是要对齐到16B

    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // 解绑

    update_light_uniformBuffer();  // 更新灯光组数据}

    GL_CHECK();
}

void ck::LightGroup::update_light_uniformBuffer() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, lights_UBO);
    int stride = Light::calculate_memory_occupancy();

    // 更新numLights
    int numLights = lights.size();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &numLights);
    // FIXME - 前16B = 4B整数 + 12B空填充

    if (lights.empty())
    {
        LOG(ERROR)
            << "Warning: No light in the light group, no need to update the light uniform buffer.";
        return;  // 没有灯光，不需要更新
    }

    // 获取指向buffer的指针
    auto* ptr = static_cast<unsigned char*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));

    // 逐元素更新buffer
    for (int i = 0; i < lights.size(); i++)
    {
        GLubyte* lightPtr = ptr + i * stride + 16;  // FIXME - +16意味着缓冲的前16B是int型的灯光数量
        lights[i].update_light_uniformBuffer(lightPtr);
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // 解绑

    GL_CHECK();
}

void ck::LightGroup::binding_uniformBuffer(uint32_t binding_point) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, lights_UBO);
}

void ck::LightGroup::binding_uniformBuffer(uint32_t binding_point,
                                           uint32_t offset,
                                           uint32_t size) const
{
    glBindBufferRange(GL_UNIFORM_BUFFER, binding_point, lights_UBO, offset, size);
}

[[nodiscard]] uint32_t ck::LightGroup::calculate_memory_occupation() const
{
    return lights.size() * Light::calculate_memory_occupancy();
}

void ck::LightGroup::print_bufferData() const
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
    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // 解绑
}
