# imgui
set(ENV{imgui_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
message("imgui_DIR: $ENV{imgui_DIR}")
# FIXME - 设置成环境变量才能在不同分支的CMakeLists.txt中使用
file(GLOB imgui_INCLUDE $ENV{imgui_DIR}/*.h)
file(GLOB imgui_SRC $ENV{imgui_DIR}/*.cpp $ENV{imgui_DIR}/*.h)
add_library(imgui STATIC ${imgui_SRC})
