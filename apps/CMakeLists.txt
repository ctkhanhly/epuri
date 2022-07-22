cmake_minimum_required(VERSION 3.0)

project(EPURI VERSION 0.0.0 LANGUAGES C)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -D_GNU_SOURCE")
add_subdirectory(simplenet)
add_subdirectory(simplenoti)
add_subdirectory(simplelogger)
add_subdirectory(fssl)

add_executable (epuri epuri.c)
target_link_libraries (epuri LINK_PUBLIC simplenet)