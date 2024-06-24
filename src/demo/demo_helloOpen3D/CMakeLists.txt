# demo_HelloOpen3D
find_package(glm CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
set(Open3D_ROOT 
    ${CMAKE_SOURCE_DIR}/3rdparty/open3d-debug-0.18.0/CMAKE)
find_package(Open3D CONFIG REQUIRED)
message(STATUS "Open3D_INCLUDE_DIRS: ${Open3D_INCLUDE_DIRS}")
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable(demo_HelloOpen3D 
    ${SRC}
    ${HEADER})
target_include_directories(demo_HelloOpen3D PRIVATE
    ${Open3D_INCLUDE_DIRS}
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(demo_HelloOpen3D PRIVATE 
    ckCore
    glog::glog
    assimp::assimp
    glm::glm
    ${Open3D_LIBRARIES}
    Eigen3::Eigen
    util)
add_custom_command(TARGET demo_HelloOpen3D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${Open3D_ROOT}/../bin/Open3D.dll"
    $<TARGET_FILE_DIR:demo_HelloOpen3D>)
install(TARGETS demo_HelloOpen3D 
    RUNTIME DESTINATION ./demo
    LIBRARY DESTINATION ./demo
    ARCHIVE DESTINATION ./demo)

#FIXME - 错图本
#release的程序不可以用debug的包
#debug的程序不可以用release的包
#遇到问题可以把整个build/删掉重构试看看
