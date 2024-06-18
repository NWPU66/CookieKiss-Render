# imgui
set(ENV{imgui_dockable_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
# FIXME - 设置成环境变量才能在不同分支的CMakeLists.txt中使用
file(GLOB ENV{imgui_dockable_INCLUDE} $ENV{imgui_dockable_DIR}/*.h)
file(GLOB imgui_dockable_SRC $ENV{imgui_dockable_DIR}/*.cpp $ENV{imgui_dockable_DIR}/*.h)
add_library(imgui_dockable STATIC ${imgui_dockable_SRC})
message(STATUS "imgui_dockable_SRC: $ENV{imgui_dockable_INCLUDE}")

# glad
# set(glad_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glad)
# add_library(glad STATIC ${glad_DIR}/src/glad.c ${glad_DIR}/include/glad/glad.h)
