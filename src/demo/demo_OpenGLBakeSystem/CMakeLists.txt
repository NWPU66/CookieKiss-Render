# demo_OpenGLBakeSystem
find_package(glm CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable(demo_OpenGLBakeSystem 
    ${SRC}
    ${HEADER})
target_include_directories(demo_OpenGLBakeSystem PRIVATE
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(demo_OpenGLBakeSystem PRIVATE 
    ckCore
    glog::glog
    assimp::assimp
    glm::glm
    util)
# add_custom_command(TARGET demo_OpenGLBakeSystem POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#     "${Open3D_ROOT}/../bin/Open3D.dll"
#     $<TARGET_FILE_DIR:demo_OpenGLBakeSystem>)
install(TARGETS demo_OpenGLBakeSystem 
    RUNTIME DESTINATION ./demo
    LIBRARY DESTINATION ./demo
    ARCHIVE DESTINATION ./demo)
