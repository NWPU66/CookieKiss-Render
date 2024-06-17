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
#include <string>
#include <vector>

#include <glad/glad.h>  //GLAD first

#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include <glm/matrix.hpp>
#include <glog/logging.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb_image.h>
/**FIXME - 关于 stb_image
如果定义了STB_IMAGE_IMPLEMENTATION，生成的中间文件就会包含stb_image的实现。
我们在model.cpp和main.cpp中都定义了STB_IMAGE_IMPLEMENTATION，
所以model.cpp.obj和main.cpp.obj都包含stb_image的源码。

解决方法：不再model.cpp中声明STB_IMAGE_IMPLEMENTATION，只在main.cpp中声明。
这样，model.cpp.obj中只有stb_image的声明，而实现在main.cpp.obj中。
*/

#include "camera.h"
#include "core/ck_debug.h"
#include "imgui_glfw_window_base.h"
#include "light.h"
#include "model.h"
#include "shader.h"

// global variable
static const std::string stdAsset_root = "E:/Study/CodeProj/CookieKiss-Render/asset/";
static const std::string asset_root =
    "E:/Study/CodeProj/CookieKiss-Render/asset/asset_demo_ShadowWithMutiLights/";
static std::array<float, 4> clear_color = {0.2F, 0.3F, 0.3F, 1.0F};
static ck::Camera           camera(glm::vec3(0.0F, 0.5F, -5.0F));

/**FIXME - 有关imgui窗口lose focus的问题
imgui_impl_glfw.cpp中设置了imgui需要glfw的回调函数
当我从遨游模式回到编辑模式时，我重新设置了glfw的回调函数，导致回调函数和
最开始imgui设置的回调函数不一致。因此imgui lose focus。
*/

inline void processInput(ck::ImguiGlfwWindowBase& window)
{
    GLFWwindow*    window_ptr = window.get_window();
    const ImGuiIO* imgui_io   = window.get_imgui_io();

    // 当Esc按下时，窗口关闭
    if (glfwGetKey(window_ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_ptr, 1);
    }

    // 根据窗口大小更新glviewport
    int32_t width  = 0;
    int32_t height = 0;
    glfwGetFramebufferSize(window_ptr, &width, &height);
    glViewport(0, 0, width, height);

    // 切换遨游模式和编辑模式
    if (glfwGetMouseButton(window_ptr, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS &&
        !window.get_isFlying())
    {
        // 按下鼠标右键，并且上一帧不在遨游模式时，进入遨游模式
        window.set_flying_mode(true);

        // 隐藏光标
        glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (glfwGetMouseButton(window_ptr, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE &&
             window.get_isFlying())
    {
        // 松开鼠标右键，并且上一帧在遨游模式中时，回到编译模式
        window.set_flying_mode(false);

        // 隐藏光标
        glfwSetInputMode(window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }  // 其他情况保持不变

    if (window.get_isFlying())
    {
        // 更新摄像机的位置和视场
        camera.process_mouse_movement(imgui_io->MouseDelta.x, imgui_io->MouseDelta.y);
        camera.process_mouse_scroll(imgui_io->MouseWheel);

        // 按下Shift时，飞行加速
        camera.speed_up((glfwGetKey(window_ptr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS));
        // 处理摄像机移动
        std::array<int32_t, 6> direction = {0, 0, 0, 0, 0, 0};
        direction[0] = (glfwGetKey(window_ptr, GLFW_KEY_W) == GLFW_PRESS) ? 1 : 0;
        direction[1] = (glfwGetKey(window_ptr, GLFW_KEY_S) == GLFW_PRESS) ? 1 : 0;
        direction[2] = (glfwGetKey(window_ptr, GLFW_KEY_A) == GLFW_PRESS) ? 1 : 0;
        direction[3] = (glfwGetKey(window_ptr, GLFW_KEY_D) == GLFW_PRESS) ? 1 : 0;
        direction[4] = (glfwGetKey(window_ptr, GLFW_KEY_E) == GLFW_PRESS) ? 1 : 0;
        direction[5] = (glfwGetKey(window_ptr, GLFW_KEY_Q) == GLFW_PRESS) ? 1 : 0;
        camera.process_keyboard(direction, imgui_io->DeltaTime);
    }
}

uint32_t create_image_bufferObject(const std::string& file_path,
                                   const bool         gamma_correction = false)
{
    // 读取
    int32_t width      = 0;
    int32_t height     = 0;
    int32_t nrChannels = 0;
    stbi_set_flip_vertically_on_load(1);  // 加载图片时翻转y轴
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &nrChannels, 0);

    // 创建纹理对象
    uint32_t texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 设置纹理属性
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 生成纹理
    if (data != nullptr)
    {
        // 设置纹理内部格式
        int32_t internal_format = 0;
        GLenum  format          = 0;
        if (nrChannels == 1)
        {
            internal_format = GL_RED;
            format          = GL_RED;
        }
        else if (nrChannels == 3)
        {
            format = GL_RGB;
            if (gamma_correction) { internal_format = GL_SRGB; }
            else { internal_format = GL_RGB; }
        }
        else if (nrChannels == 4)
        {
            format = GL_RGBA;
            if (gamma_correction) { internal_format = GL_SRGB_ALPHA; }
            else { internal_format = GL_RGBA; }
        }
        else { LOG(WARNING) << "no sutibale format for texture: " << file_path; }

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE,
                     data);
        //  设置为GL_SRGB时，OpenGL回自动对图片进行重校
        // FIXME - 注意，对于在线性空间下创建的纹理，如法线贴图，不能设置SRGB重校。
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOG(WARNING) << "Failed to load texture";
        return 0;
    }

    stbi_image_free(data);            // 释放图像的内存，无论有没有data都释放
    glBindTexture(GL_TEXTURE_2D, 0);  // 解绑
    return texture;
}

int main(int argc, char** argv)
{
    // init glog
    google::InitGoogleLogging(*argv);
    FLAGS_minloglevel = google::LogSeverity::GLOG_INFO;  // 设置最小日志级别
    // FLAGS_log_dir                   = "./log";                         // 设置日志目录
    // FLAGS_stop_logging_if_full_disk = true;  // 设置磁盘满时停止写日志

    // create glfw window
    ck::ImguiGlfwWindowBase window({1280, 720}, "ShadowWithMutiLights");

    // init glad
    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0)
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // glfw opengl debug
#ifdef NDEBUG
#else
    int32_t flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if ((flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0)
    {
        LOG(INFO) << "GL_CONTEXT_FLAG_DEBUG_BIT is enabled.";
        glDebugMessageCallback(ck::glDebugOutput, nullptr);  // 向opengl注册调试回调函数
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR,
                              GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_TRUE);
    }
    else { LOG(WARNING) << "GL_CONTEXT_FLAG_DEBUG_BIT is not enabled."; }
#endif

    // ANCHOR - opengl setting
    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);                            // 启用深度缓冲
    glDepthFunc(GL_LEQUAL);                             // 修改深度测试的标准
    glEnable(GL_STENCIL_TEST);                          // 启用模板缓冲
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);       // 设置模板缓冲的操作
    glEnable(GL_BLEND);                                 // 启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // 设置混合函数
    glEnable(GL_CULL_FACE);                             // 启用面剔除
    glEnable(GL_MULTISAMPLE);                           // 启用多重采样
    glEnable(GL_FRAMEBUFFER_SRGB);                      // 自动Gamme矫正
    GL_CHECK();

    // ANCHOR -  init my asset
    ck::Model  cube(asset_root + "box.obj");
    ck::Model  plane(asset_root + "plane.obj");
    ck::Model  sphere(stdAsset_root + "stdModel/sphere/sphere.obj");
    ck::Shader shadowed_phong(stdAsset_root + "stdShader/stdVerShader.vs.glsl",
                              asset_root + "stdShadowedPhongLighting.fs.glsl");
    ck::Shader lightObjShader(stdAsset_root + "stdShader/stdVerShader.vs.glsl",
                              stdAsset_root + "stdShader/stdPureColor.fs.glsl");
    GL_CHECK();

    // 灯光组
    ck::LightGroup         light_group;
    std::vector<ck::Light> lights = {
        ck::Light(0, glm::vec3(1, 1, 1), 2, glm::vec3(1, 1.5, 1)),
        ck::Light(1, glm::vec3(1, 1, 1), 1.2, glm::vec3(0, 0, 0), glm::vec3(1, -1, 1)),
        ck::Light(2, glm::vec3(1, 1, 1), 1, glm::vec3(0, 1.5, 0), glm::vec3(0, -1, 0))};
    light_group.add_light(lights);
    light_group.create_light_uniformBuffer();
    light_group.binding_uniformBuffer(0);
    GL_CHECK();

    // main loop
    while (glfwWindowShouldClose(window.get_window()) == 0)
    {
        glfwPollEvents();
        processInput(window);

        // ANCHOR -  Start the Dear ImGui frame
        {
            static bool open_demo_window = false;

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("CookieKiss Render Assistant");
            ImGui::ColorEdit3("clear color", clear_color.data());
            if (ImGui::Button("open Demo window")) { open_demo_window = true; }
            ImGui::End();

            if (open_demo_window) { ImGui::ShowDemoWindow(&open_demo_window); }
        }
        ImGui::Render();

        // ANCHOR -  OpenGL rendering goes here
        {
            // clear
            glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            // view and projection
            int32_t window_width  = 0;
            int32_t window_height = 0;
            glfwGetFramebufferSize(window.get_window(), &window_width, &window_height);
            const float camera_aspect_ratio =
                static_cast<float>(window_width) / static_cast<float>(window_height);
            const glm::mat4 view       = camera.get_view_matrix();
            const glm::mat4 projection = glm::perspective(glm::radians(camera.get_camera_zoom()),
                                                          camera_aspect_ratio, 0.1F, 100.0F);

            // cube
            shadowed_phong.use();
            shadowed_phong.setParameter(
                "model",
                glm::translate(glm::scale(glm::mat4(1), glm::vec3(0.5)), glm::vec3(0, 1, 0)));
            shadowed_phong.setParameter("view", view);
            shadowed_phong.setParameter("projection", projection);
            shadowed_phong.setParameter("cameraPos", camera.get_position());
            cube.draw(shadowed_phong);
            /**FIXME - 问题记录：
            cube的顶点是对的，问题在片元着色器上
            1. 相机移动不了，回调函数没有被调用。
            原因：填给回调函数的是函数的地址，是复制地址。还有是时钟没更新
            还有一个原因是，yaw被锁定了（限制在-89到89度之间），大概代码抄错了。

            2. cube有几个面是全黑的，怀疑是uv有问题。
            应该是光照的问题，有三个面在方向光源的背面。
             */
            // ground
            shadowed_phong.use();
            shadowed_phong.setParameter("model", glm::scale(glm::mat4(1), glm::vec3(5)));
            plane.draw(shadowed_phong);

            // light group
            lightObjShader.use();
            lightObjShader.setParameter("view", view);
            lightObjShader.setParameter("projection", projection);
            for (const auto& light : light_group.get_lights())
            {
                if (light.get_light_type() != 1)  // 日光不渲染实体
                {
                    lightObjShader.setParameter(
                        "model", glm::scale(glm::translate(glm::mat4(1), light.get_postion()),
                                            glm::vec3(0.1)));
                    lightObjShader.setParameter("lightColor", light.get_color());
                    sphere.draw(lightObjShader);
                    // FIXME - 常量对象只能调用它的常函数
                }
            }
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window.get_window());
        GL_CHECK();
    }
    // clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window.get_window());
    glfwTerminate();
    // NOTE - 这里GL_CHECK()没问题，大概是oprngl上下文被删除了。

    google::ShutdownGoogleLogging();
    return EXIT_SUCCESS;
}

/**FIXME - 课外知识
    virtual float f3() const && noexcept = delete;
    虚函数，const表示常成员函数
    &&表示只能被右值引用的对象调用
    noexcept表示不会抛出异常
 */
