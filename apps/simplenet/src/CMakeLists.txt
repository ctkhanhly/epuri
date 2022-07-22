cmake_minimum_required(VERSION 3.0)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

add_library (simplenet SHARED net.c client.c server.c   "${PROJECT_SOURCE_DIR}/include/simplenet/net.h"
                                                        "${PROJECT_SOURCE_DIR}/include/simplenet/client.h"
                                                        "${PROJECT_SOURCE_DIR}/include/simplenet/server.h")

target_include_directories(simplenet
                           PUBLIC
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                           "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
                           )
include_directories("$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")
link_directories("${CMAKE_SOURCE_DIR}/simplelogger/src")
target_link_libraries(simplenet LINK_PUBLIC simplelogger)