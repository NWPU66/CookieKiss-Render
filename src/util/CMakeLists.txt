set(UTIL_DIR ${CMAKE_CURRENT_LIST_DIR})
file(GLOB UTIL_SRC ${UTIL_DIR}/*.cpp)
file(GLOB UTIL_HEADER ${UTIL_DIR}/*.h)
add_library(util STATIC ${UTIL_SRC} ${UTIL_HEADER})
