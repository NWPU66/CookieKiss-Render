# imgui
set(imgui_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(GLOB imgui_INCLUDE ${imgui_DIR}/*.h)
file(GLOB imgui_SRC ${imgui_DIR}/*.cpp ${imgui_DIR}/*.h)
add_library(imgui STATIC ${imgui_SRC})
