find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

if( OPENSSL_FOUND )

     include_directories(${OPENSSL_INCLUDE_DIRS})
     message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
  # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

add_library(fssl SHARED src/ssl.c)
target_link_libraries (fssl LINK_PUBLIC ssl)

# add_library(simplelogger SHARED IMPORTED)
# set_target_properties(simplelogger PROPERTIES
#     IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/simplelogger/src/simplelogger.so"
#     INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/simplelogger/include"
# )
# target_link_libraries(simplenet simplelogger)

# add_library(simplenet SHARED IMPORTED)
# set_target_properties(simplenet PROPERTIES
#     IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/simplenet/src/simplenet.so"
#     INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/simplenet/include"
# )
# target_link_libraries(simplenet simplenet)

include_directories("$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")
link_directories("${CMAKE_SOURCE_DIR}/simplelogger/src")
target_link_libraries(fssl LINK_PUBLIC simplelogger)

link_directories("${CMAKE_SOURCE_DIR}/simplenet/src")
target_link_libraries(fssl LINK_PUBLIC simplenet)