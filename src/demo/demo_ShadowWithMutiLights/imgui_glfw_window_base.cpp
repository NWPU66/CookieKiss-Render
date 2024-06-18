#include "imgui_glfw_window_base.h"

#include <cstdint>
#include <stdexcept>

#include <string>

#include <GLFW/glfw3.h>
#include <glog/logging.h>
#include <imgui.h>

ck::ImguiGlfwWindowBase::ImguiGlfwWindowBase(const std::array<int32_t, 2> window_size,
                                             const std::string&           title,
                                             const std::array<int32_t, 2> version)
    : framebuffer_size_callback(nullptr), mouse_callback(nullptr), scroll_callback(nullptr),
      flying_mode(false)
{
    // 创建GLFW窗口，版本：GL 4.6 + GLSL 460
    {
        if (glfwInit() == 0) { throw std::runtime_error("glfwInit failed!"); }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version[0]);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version[1]);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        glfwWindowHint(GLFW_SAMPLES, 4);                     // 多重采样缓冲
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // glfw增加调试输出

        window = glfwCreateWindow(window_size[0], window_size[1], title.c_str(), nullptr, nullptr);
        if (window == nullptr) { throw std::runtime_error("glfwCreateWindow failed!"); }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);  // Enable vsync，垂直同步
    }

    // 初始化imgui
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        imgui_io = &ImGui::GetIO();
        (void)(*imgui_io);  // 将 io 转换为 void 类型并丢弃，从而避免编译器警告？？？
        if (imgui_io == nullptr) { throw std::runtime_error("imgui_io is nullptr!"); }

        imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
        imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const std::string glsl_version =
            "#version " + std::to_string(version[0]) + std::to_string(version[1]) + "0";
        ImGui_ImplOpenGL3_Init(glsl_version.c_str());
    }
}

GLFWwindow* ck::ImguiGlfwWindowBase::get_window() const
{
    if (window == nullptr) { throw std::runtime_error("window is nullptr!"); }
    return window;
}

const ImGuiIO* ck::ImguiGlfwWindowBase::get_imgui_io() const
{
    if (imgui_io == nullptr) { throw std::runtime_error("imgui_io is nullptr!"); }
    return imgui_io;
}

[[nodiscard]] bool ck::ImguiGlfwWindowBase::get_isFlying() const
{
    return flying_mode;
}

void ck::ImguiGlfwWindowBase::set_flying_mode(const bool _flying_mode)
{
    flying_mode = _flying_mode;
}

void ck::ImguiGlfwWindowBase::set_glfwWindow_callBack(
    void (*_framebuffer_size_callback)(GLFWwindow*, int, int),
    void (*_mouse_callback)(GLFWwindow*, double, double),
    void (*_scroll_callback)(GLFWwindow*, double, double))
{
    // if (_framebuffer_size_callback == nullptr || _mouse_callback == nullptr ||
    //     _scroll_callback == nullptr)
    // {
    //     LOG(ERROR) << "framebuffer_size_callback, mouse_callback or scroll_callback is nullptr!";
    //     throw std::runtime_error(
    //         "framebuffer_size_callback, mouse_callback or scroll_callback is nullptr!");
    // }

    // 现在这几枚函数指针可以为nullptr

    framebuffer_size_callback = _framebuffer_size_callback;
    mouse_callback            = _mouse_callback;
    scroll_callback           = _scroll_callback;

    // 注册回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
}
