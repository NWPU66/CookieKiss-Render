cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_TOOLCHAIN_FILE "E:/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_INCLUDE_DIR "E:/vcpkg/installed/x64-windows/include")

set(CMAKE_CXX_STANDARD 17)
project(CookieKissRender
    VERSION 0.1.0
    DESCRIPTION "CookieKissRender"
    HOMEPAGE_URL "https://github.com/NWPU66/CookieKiss-Render"
    LANGUAGES CXX)

# add subdirectory
subdirs(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
subdirs(${CMAKE_CURRENT_SOURCE_DIR}/src)

# install path
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
