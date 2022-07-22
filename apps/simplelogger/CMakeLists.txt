cmake_minimum_required(VERSION 3.0)

project(SIMPLELOGGER_LIB VERSION 0.0.0 LANGUAGES C)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

set(SIMPLELOGGER_LIB simplelogger)
add_subdirectory(src)