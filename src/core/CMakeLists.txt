# CookieKiss System Core Code
find_package(glad CONFIG REQUIRED)
find_package(glog CONFIG REQUIRED)
file(GLOB core_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB core_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(ckCore ${core_SRC} ${core_HEADER})
target_include_directories(ckCore PUBLIC 
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(ckCore PUBLIC 
    glad::glad
    glog::glog
    util)
install(TARGETS ckCore 
    LIBRARY DESTINATION ./ckCore
    ARCHIVE DESTINATION ./ckCore)
