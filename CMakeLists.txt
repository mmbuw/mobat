cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (udp)

# sources
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
# output executables
add_executable(udp_test ${SOURCES})
# target_link_libraries(udp_test recorder signaltrans locator visualization testing -lpthread)
# if(ODROID_BUILD)
#   target_link_libraries(udp_test -lX11 -lX11-xcb)
# endif()

#request C++11 if on unix, stop at all warnings but ignore specific warning in glm headers
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
if(UNIX)
  add_definitions( -std=c++11 -Wall -Wno-strict-aliasing)
endif(UNIX)

#set the build type if its not set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE )
endif()

#if user didnt set install dir, override it and write it to the cache -> Type and description necessary,
#overwrite variable, not just write it to cache
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)