cmake_minimum_required(VERSION 3.0)

project(SIMPLENET VERSION 0.0.0 LANGUAGES C)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(SIMPLENET_LIB_NAME simplenet)

add_subdirectory(src)