# demo_HelloOpenGL
find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
add_executable(demo_HelloOpenGL
    main.cpp
    $ENV{imgui_DIR}/backends/imgui_impl_glfw.cpp 
    $ENV{imgui_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(demo_HelloOpenGL PRIVATE
    $ENV{imgui_DIR}/backends
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(demo_HelloOpenGL PRIVATE 
    imgui
    util 
    glm::glm
    glfw
    glad::glad)
install(TARGETS demo_HelloOpenGL RUNTIME DESTINATION ./demo)
