# demo_ShadowWithMutiLights
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable(demo_ShadowWithMutiLights 
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
    ${SRC}
    ${HEADER})
target_include_directories(demo_ShadowWithMutiLights PRIVATE
    # ${CMAKE_SOURCE_DIR}/3rdparty/imgui
    ${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(demo_ShadowWithMutiLights PRIVATE 
    ckCore
    glog::glog
    glfw
    assimp::assimp
    imgui::imgui
    glm::glm
    glad::glad
    util)
install(TARGETS demo_ShadowWithMutiLights 
    RUNTIME DESTINATION ./demo
    LIBRARY DESTINATION ./demo
    ARCHIVE DESTINATION ./demo)
