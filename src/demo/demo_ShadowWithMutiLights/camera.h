#pragma once

#include <cstdint>

#include <array>
#include <glm/ext/vector_float3.hpp>

#include "glm/glm.hpp"

static const float DEFAULT_MOVE_SPEED        = 5.0F;
static const float DEFAULT_MOUSE_SENSITIVITY = 0.1F;

namespace ck {

class Camera {
private:
    glm::vec3 position;
    glm::vec3 front_vec;
    glm::vec3 up_vec;
    glm::vec3 right_vec;
    glm::vec3 world_up_vec;

    float yaw;
    float pitch;

    float move_speed;
    float mouse_sensitivity;
    float camera_zoom;

    bool first_frame_to_view;

    void update_camera_vector();

public:
    explicit Camera(const glm::vec3& position     = glm::vec3(0.0F, 0.0F, 0.0F),
                    const glm::vec3& world_up_vec = glm::vec3(0.0F, 1.0F, 0.0F));

    [[nodiscard]] glm::vec3 get_position() const;
    [[nodiscard]] float     get_camera_zoom() const;
    [[nodiscard]] glm::mat4 get_view_matrix() const;

    void process_keyboard(const std::array<int32_t, 6>& directions, float delta_time);
    void process_mouse_movement(float x_offset, float y_offset, bool constarinPitch = true);
    void process_mouse_scroll(float y_offset);
    void speed_up(bool is_speed_up);
};

};  // namespace ck
