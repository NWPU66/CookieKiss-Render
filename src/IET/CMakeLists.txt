# IET
add_executable(IET main.cpp)
target_include_directories(IET PRIVATE
    ${VCPKG_INCLUDE_DIR}
    ${SRC_DIR})
target_link_libraries(IET PRIVATE 
    util)
install(TARGETS IET 
    RUNTIME DESTINATION ./IET
    LIBRARY DESTINATION ./IET
    ARCHIVE DESTINATION ./IET)
