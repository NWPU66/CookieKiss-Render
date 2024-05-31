cmake_minimum_required(VERSION 3.25)
set(CMAKE_TOOLCHAIN_FILE "E:/vcpkg/scripts/buildsystems/vcpkg.cmake")
project(demo_HelloOpenGL)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(imgui REQUIRED)
find_package(glog REQUIRED)
message(STATUS "OpenGL_FOUND:${OpenGL_FOUND}")
message(STATUS "glfw3_FOUND:${glfw3_FOUND}")
message(STATUS "glog_FOUND:${glog_FOUND}")

include_directories(
"E:/Study/CodeProj/CookieKiss-Render/utilTool" 
"E:/Study/CodeProj/CookieKiss-Render/3rdparty/imgui/backends")

file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR}/*.cpp)
message(STATUS "SRC:${SRC}")
add_executable(demo_HelloOpenGL ${SRC})