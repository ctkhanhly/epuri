cmake_minimum_required(VERSION 3.0)

project(SIMPLE_NOTI_LIB VERSION 0.0.0 LANGUAGES C)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(SIMPLE_NOTI_LIB simplenoti)
add_subdirectory(src)