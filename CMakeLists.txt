cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (TTT)

#add include folder
include_directories(include/recorder
					include/signaltransform)

# recorder
file(GLOB RECORDER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/recorder/*.cpp)
add_library(recorder STATIC ${RECORDER_SOURCES})
target_link_libraries(recorder -lasound)

# output executable
add_executable(ttt src/ttt.cpp)
target_link_libraries(ttt -lasound)
target_link_libraries(ttt recorder)

#request C++11 if on unix
if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Werror")
endif(UNIX)

#if user didnt set install dir, override it and write it to the cache -> Type and description necessary,
#overwrite variable, not just write it to cache
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# installation rules, copy over binary to bin
install(TARGETS ttt DESTINATION bin)