# demo_HelloOpenGL
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
add_executable(demo_HelloOpenGL
    main.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.cpp 
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(demo_HelloOpenGL PRIVATE
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(demo_HelloOpenGL PRIVATE 
    imgui::imgui
    glfw
    glad::glad
    util)
install(TARGETS demo_HelloOpenGL 
    RUNTIME DESTINATION ./demo
    LIBRARY DESTINATION ./demo
    ARCHIVE DESTINATION ./demo)
