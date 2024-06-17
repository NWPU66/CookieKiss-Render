#include "camera.h"

#include <algorithm>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/ext/vector_float3.hpp>

ck::Camera::Camera(const glm::vec3& position, const glm::vec3& world_up_vec)
    : position(position), world_up_vec(world_up_vec), yaw(90.0F), pitch(0.0F),
      front_vec(glm::vec3(0)), move_speed(DEFAULT_MOVE_SPEED),
      mouse_sensitivity(DEFAULT_MOUSE_SENSITIVITY), camera_zoom(45.0F), skip_one_frame_flag(true)
{
    update_camera_vector();
}

[[nodiscard]] glm::vec3 ck::Camera::get_position() const
{
    return position;
}

[[nodiscard]] float ck::Camera::get_camera_zoom() const
{
    return camera_zoom;
}

[[nodiscard]] glm::mat4 ck::Camera::get_view_matrix() const
{
    return glm::lookAt(position, position + front_vec, up_vec);
}

void ck::Camera::set_skip_one_frame(const bool flag)
{
    skip_one_frame_flag = flag;
}

void ck::Camera::process_keyboard(const std::array<int32_t, 6>& directions, const float delta_time)
{
    float velocity = move_speed * delta_time;
    position += front_vec * velocity * static_cast<float>(directions[0]);
    position -= front_vec * velocity * static_cast<float>(directions[1]);
    position -= right_vec * velocity * static_cast<float>(directions[2]);
    position += right_vec * velocity * static_cast<float>(directions[3]);
    position += world_up_vec * velocity * static_cast<float>(directions[4]);
    position -= world_up_vec * velocity * static_cast<float>(directions[5]);
}

void ck::Camera::process_mouse_movement(const float x_offset,
                                        const float y_offset,
                                        const bool  constarinPitch)
{
    if (skip_one_frame_flag)
    {
        skip_one_frame_flag = false;
        return;
    }

    yaw += x_offset * mouse_sensitivity;
    pitch -= y_offset * mouse_sensitivity;
    if (constarinPitch) { pitch = std::clamp(pitch, -89.9F, 89.9F); }
    update_camera_vector();
}

void ck::Camera::process_mouse_scroll(const float y_offset)
{
    camera_zoom = std::clamp(camera_zoom - y_offset, 1.0F, 45.0F);
}

void ck::Camera::speed_up(bool is_speed_up)
{
    move_speed = (is_speed_up) ? DEFAULT_MOVE_SPEED * 3 : DEFAULT_MOVE_SPEED;
}

void ck::Camera::update_camera_vector()
{
    front_vec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front_vec.y = sin(glm::radians(pitch));
    front_vec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front_vec   = glm::normalize(front_vec);

    right_vec = glm::normalize(glm::cross(front_vec, world_up_vec));
    up_vec    = glm::normalize(glm::cross(right_vec, front_vec));
}
