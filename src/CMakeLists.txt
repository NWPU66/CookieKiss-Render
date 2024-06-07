set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# add sub directories
subdirs(
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/demo
    ${CMAKE_CURRENT_SOURCE_DIR}/IET
    ${CMAKE_CURRENT_SOURCE_DIR}/util
)

# helloworld programe
find_package(glm CONFIG REQUIRED)
add_executable(helloworld helloworld.cpp)
target_include_directories(helloworld PRIVATE
    ${imgui_INCLUDE}
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(helloworld PRIVATE 
    imgui
    util 
    glm::glm)
install(TARGETS helloworld RUNTIME DESTINATION ./)
