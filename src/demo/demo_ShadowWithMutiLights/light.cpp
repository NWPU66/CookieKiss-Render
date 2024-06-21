#include "light.h"

#include <cstdint>

#include <iostream>

#include <glad/glad.h>  //glad first

#include <glog/logging.h>

#include "core/ck_debug.h"

ck::Light::Light(const int32_t   light_type,
                 const glm::vec3 color,
                 const float     intensity,
                 const float     inner_cutOff,
                 const float     outer_cutOff)
    : light_type(light_type), color(color), intensity(intensity), inner_cutOff(inner_cutOff),
      outer_cutOff(outer_cutOff)
{
}

[[nodiscard]] int32_t ck::Light::get_light_type() const
{
    return light_type;
}

[[nodiscard]] glm::vec3 ck::Light::get_color() const
{
    return color;
}

int32_t ck::Light::calculate_memory_occupancy()
{
    /**NOTE - memory occupation
     * GLint     lightType;（4B）
     * glm::vec3 color;（16B）
     * GLfloat   intensity;（4B）
     * GLfloat innerCutOff, outerCutOf,;（4B，4B）
     */
    /**FIXME - 错题本
     * std140的布局理解错了！
     * 详细的计算分析请看 opengl/src/advancedLighting/memoryLayout.md
     */
    return 80;
    // NOTE - 还是按照原来的填充方案，只不过position和rotation的数据从外面送进来
}

void ck::Light::update_light_uniformBuffer(unsigned char* const ptr,
                                           const glm::vec3&     position,
                                           const glm::vec3&     rotation) const
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
