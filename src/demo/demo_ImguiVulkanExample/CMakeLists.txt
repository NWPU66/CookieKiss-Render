# demo_ImguiVulkanExample
find_package(glfw3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(imgui CONFIG REQUIRED)
add_executable(demo_ImguiVulkanExample 
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_vulkan.cpp
    main.cpp)
target_include_directories(demo_ImguiVulkanExample PRIVATE
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(demo_ImguiVulkanExample PRIVATE 
    imgui::imgui
    Vulkan::Vulkan
    glfw
    util)
install(TARGETS demo_ImguiVulkanExample 
    RUNTIME DESTINATION ./demo
    LIBRARY DESTINATION ./demo
    ARCHIVE DESTINATION ./demo)
