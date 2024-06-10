#pragma once

#include <cstdint>

#include <array>
#include <functional>
#include <string>

#include <glad/glad.h>  //GLAD first

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace ck {

class ImguiGLfwWindowBase {
private:
    GLFWwindow* window;
    ImGuiIO*    imgui_io;

    // callback function pointer
    void (*framebuffer_size_callback)(GLFWwindow*, int, int);
    void (*mouse_callback)(GLFWwindow*, double, double);
    void (*scroll_callback)(GLFWwindow*, double, double);

public:
    explicit ImguiGLfwWindowBase(std::array<int32_t, 2> window_size,
                                 const std::string&     title   = "window",
                                 std::array<int32_t, 2> version = {4, 6});
    ~ImguiGLfwWindowBase() = default;

    // 拷贝方法与移动方法
    ImguiGLfwWindowBase(const ImguiGLfwWindowBase&)            = default;
    ImguiGLfwWindowBase& operator=(const ImguiGLfwWindowBase&) = default;
    ImguiGLfwWindowBase(ImguiGLfwWindowBase&&)                 = default;
    ImguiGLfwWindowBase& operator=(ImguiGLfwWindowBase&&)      = default;

    // get method
    // NOTE - [[nodiscard]]表示，如果调用这个函数后忽略了它的返回值，应该生成一个警告。
    [[nodiscard]] GLFWwindow*    get_window() const;
    [[nodiscard]] const ImGuiIO* get_imgui_io() const;

    void set_glfwWindow_callBack(void (*_framebuffer_size_callback)(GLFWwindow*, int, int),
                                 void (*_mouse_callback)(GLFWwindow*, double, double),
                                 void (*_scroll_callback)(GLFWwindow*, double, double));
};

};  // namespace ck
