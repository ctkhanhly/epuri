cmake_minimum_required(VERSION 3.0)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library (simplenoti SHARED main.c common.c sandbox.c tracer.c scm_functions.c
            common.h sandbox.h scm_functions.h tracer.h)

target_link_libraries (simplenoti LINK_PUBLIC dl)
target_link_libraries (simplenoti LINK_PUBLIC seccomp)
target_link_libraries (simplenoti LINK_PUBLIC ssl)

include_directories("$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")
link_directories("${CMAKE_SOURCE_DIR}/simplelogger/src")
target_link_libraries(simplenoti LINK_PUBLIC simplelogger)