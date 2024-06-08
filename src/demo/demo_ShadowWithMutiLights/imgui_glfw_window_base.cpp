#include "imgui_glfw_window_base.h"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <string>

ck::ImguiGLfwWindowBase::ImguiGLfwWindowBase(const std::array<int32_t, 2> window_size,
                                             const std::string&           title,
                                             const std::array<int32_t, 2> version)
{
    // 创建GLFW窗口，版本：GL 4.6 + GLSL 460
    {
        if (glfwInit() == 0) { throw std::runtime_error("glfwInit failed!"); }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version[0]);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version[1]);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(window_size[0], window_size[1], title.c_str(), nullptr, nullptr);
        if (window == nullptr) { throw std::runtime_error("glfwCreateWindow failed!"); }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);  // Enable vsync，垂直同步}
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

ck::ImguiGLfwWindowBase::~ImguiGLfwWindowBase()
{
    // cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* ck::ImguiGLfwWindowBase::get_window() const
{
    if (window == nullptr) { throw std::runtime_error("window is nullptr!"); }
    return window;
}

const ImGuiIO* ck::ImguiGLfwWindowBase::get_imgui_io() const
{
    if (imgui_io == nullptr) { throw std::runtime_error("imgui_io is nullptr!"); }
    return imgui_io;
}
