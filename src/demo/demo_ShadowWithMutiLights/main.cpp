/**NOTE -  #include 的路径及顺序
 * dir2/foo2.h.
 * C 语言系统文件 (确切地说: 用使用方括号和 .h 扩展名的头文件), 例如 <unistd.h> 和 <stdlib.h>.
 * C++ 标准库头文件 (不含扩展名), 例如 <algorithm> 和 <cstddef>.
 * 其他库的 .h 文件.
 * 本项目的 .h 文件.
 */

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <array>
#include <iostream>
#include <vector>

#include "glad/glad.h"  //GLAD first

#include "GLFW/glfw3.h"
#include "glm/matrix.hpp"
#include "glog/logging.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imgui_glfw_window_base.h"

// global variable
static const int32_t        window_width  = 800;
static const int32_t        window_height = 600;
static std::array<float, 4> clear_color   = {0.2F, 0.3F, 0.3F, 1.0F};

int main(int argc, char** argv)
{
    // init glog
    google::InitGoogleLogging(*argv);

    // create glfw window
    ck::ImguiGLfwWindowBase window({window_width, window_height}, "ShadowWithMutiLights");

    // init glad
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    LOG(INFO) << "init successfully!";
    const ImGuiIO* imgui_io = window.get_imgui_io();

    // opengl setting
    glViewport(0, 0, window_width, window_height);
    glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);

    // main loop
    while (glfwWindowShouldClose(window.get_window()) == 0)
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("rendering settings");
            ImGui::Text("This is some useful text.");
            ImGui::ColorEdit3("clear color", clear_color.data());
            ImGui::End();
        }
        ImGui::Render();

        // OpenGL rendering goes here
        {
            // ......
            int32_t width  = 0;
            int32_t height = 0;
            glfwGetFramebufferSize(window.get_window(), &width, &height);
            glViewport(0, 0, width, height);
            glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window.get_window());
    }
    // clean up
    window.~ImguiGLfwWindowBase();
    google::ShutdownGoogleLogging();
    return EXIT_SUCCESS;
}

/**FIXME - 课外知识
    virtual float f3() const && noexcept = delete;
    虚函数，const表示常成员函数
    &&表示只能被右值引用的对象调用
    noexcept表示不会抛出异常
 */
