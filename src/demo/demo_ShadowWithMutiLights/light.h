#pragma once

#include <cstdint>

#include <vector>

#include <glm/glm.hpp>

static const uint32_t MAX_LIGHTS_SUPPORTED = 16;
static const float    DEFUALT_INNER_CUTOFF = cos(glm::radians(12.5F));
static const float    DEFUALT_OUTER_CUTOFF = cos(glm::radians(17.5F));

namespace ck {

struct LightAttributes
{
    int32_t   light_type;
    glm::vec3 color;
    float     intensity;

    float inner_cutOff, outer_cutOff;  // for spot light
};

/// @param lightType -1代表无效灯，0点光，1日光，2聚光，3面光。
class Light {
private:
    int32_t   light_type;
    glm::vec3 color;
    float     intensity;

    float inner_cutOff, outer_cutOff;  // for spot light

public:
    explicit Light(int32_t   light_type   = -1,
                   glm::vec3 color        = glm::vec3(1),
                   float     intensity    = 1,
                   float     inner_cutOff = DEFUALT_INNER_CUTOFF,
                   float     outer_cutOff = DEFUALT_OUTER_CUTOFF);

    [[nodiscard]] int32_t   get_light_type() const;
    [[nodiscard]] glm::vec3 get_color() const;

    static int32_t calculate_memory_occupancy();
    void           update_light_uniformBuffer(unsigned char*   ptr,
                                              const glm::vec3& position,
                                              const glm::vec3& rotation) const;
};

};  // namespace ck