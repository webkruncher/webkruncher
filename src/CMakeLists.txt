cmake_minimum_required(VERSION 3.10)

project("webkruncher")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../krunchercore/cmake/CMakeLists.txt)

add_subdirectory( include )
add_subdirectory( site )


