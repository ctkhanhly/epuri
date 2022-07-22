cmake_minimum_required(VERSION 3.0)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library (simplelogger SHARED log.c "${PROJECT_SOURCE_DIR}/include/simplelogger/log.h")

target_include_directories(simplelogger
                           PUBLIC
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                           "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
                           )