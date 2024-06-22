# demo_helloOpen3D
find_package(glm CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
set(Open3D_ROOT 
    ${CMAKE_SOURCE_DIR}/3rdparty/open3d-debug-0.17.0/CMAKE)
find_package(Open3D CONFIG REQUIRED)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB HEADER ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable(demo_helloOpen3D 
    ${SRC}
    ${HEADER})
target_include_directories(demo_helloOpen3D PRIVATE
    ${Open3D_INCLUDE_DIRS}
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(demo_helloOpen3D PRIVATE 
    ckCore
    glog::glog
    assimp::assimp
    glm::glm
    ${Open3D_LIBRARIES}
    Eigen3::Eigen
    util)
install(TARGETS demo_helloOpen3D 
    RUNTIME DESTINATION ./demo
    LIBRARY DESTINATION ./demo
    ARCHIVE DESTINATION ./demo)
