# CookieKiss System Core Code
file(GLOB core_SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB core_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
add_library(ckCore ${core_SRC} ${core_HEADER})
target_include_directories(ckCore PUBLIC 
    ${SRC_DIR})
target_link_libraries(ckCore PUBLIC 
    util)
install(TARGETS ckCore 
    LIBRARY DESTINATION ./ckCore
    ARCHIVE DESTINATION ./ckCore)
