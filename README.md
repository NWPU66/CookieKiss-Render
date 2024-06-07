# CookieKiss-Render
![alt text](asset/stdTexture/AI3_ae_00000.png)

---
## 项目介绍 Intro
基于OpenGL的个人渲染器项目
- IntegratedEdittingEnv是集成式编辑终端（类似Blender、UE等），能够实现编辑器的简单操作功能、图形渲染以及场景物体管理。
- demo则是小项目的演示，包含：简单技术实现和来自其他仓库的示例代码。

---
## 快速上手 Quick Start

### 构建与编译
1. 准备必要的第三方库：glfw3、imgui、glm、vulkan

1.1 使用vcpkg
```
./vcpkg.exe install glfw3:x64-windowsimgui:x64-windows glm:x64-windows vulkan:x64-windows
```
1.2 手动链接
在vs或rider中，右键项目，选择属性，选择链接器，选择输入，选择加依赖项，添加库的路径。
```
/path/to/your/lib/glfw3.lib
/path/to/your/lib/imgui.lib
/path/to/your/lib/glm.lib
/path/to/your/lib/vulkan-1.lib
```

2. 编译

### 运行

---
## 项目结构 Hierarchy
```
./3rdparty
./ckCore
./demo
```

---
## 特性 Features

---
## 参考代码 Reference Code
- demo/demo_HelloOpenGL 参考imgui官方的OpenGL示例
- demo/demo_ImguiVulkanExample 参考imgui官方的Vulkan示例