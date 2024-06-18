#pragma once

#include <cstdint>

#include <array>
#include <functional>
#include <string>

#include <glad/glad.h>  //GLAD first

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>


namespace ck {

class ImguiGlfwWindowBase {
private:
    GLFWwindow* window;
    ImGuiIO*    imgui_io;
    bool        flying_mode;

    // callback function pointer
    void (*framebuffer_size_callback)(GLFWwindow*, int, int);
    void (*mouse_callback)(GLFWwindow*, double, double);
    void (*scroll_callback)(GLFWwindow*, double, double);

public:
    explicit ImguiGlfwWindowBase(std::array<int32_t, 2> window_size,
                                 const std::string&     title   = "window",
                                 std::array<int32_t, 2> version = {4, 6});
    ~ImguiGlfwWindowBase() = default;

    // 拷贝方法与移动方法
    ImguiGlfwWindowBase(const ImguiGlfwWindowBase&)            = default;
    ImguiGlfwWindowBase& operator=(const ImguiGlfwWindowBase&) = default;
    ImguiGlfwWindowBase(ImguiGlfwWindowBase&&)                 = default;
    ImguiGlfwWindowBase& operator=(ImguiGlfwWindowBase&&)      = default;

    // get method
    // NOTE - [[nodiscard]]表示，如果调用这个函数后忽略了它的返回值，应该生成一个警告。
    [[nodiscard]] GLFWwindow*    get_window() const;
    [[nodiscard]] const ImGuiIO* get_imgui_io() const;
    [[nodiscard]] bool           get_isFlying() const;

    void set_flying_mode(bool _flying_mode);

    void set_glfwWindow_callBack(void (*_framebuffer_size_callback)(GLFWwindow*, int, int),
                                 void (*_mouse_callback)(GLFWwindow*, double, double),
                                 void (*_scroll_callback)(GLFWwindow*, double, double));
};

};  // namespace ck
