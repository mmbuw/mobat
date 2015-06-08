cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (TTT)

#add include folder
include_directories(include)

# collect source files
file(GLOB TTT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

# output executable
add_executable(ttt ${TTT_SOURCES})

#request C++11 if on unix
if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
endif(UNIX)

# link the exe with the sound library
target_link_libraries(ttt -lasound)

#if user didnt set install dir, override it and write it to the cache -> Type and description necessary,
#overwrite variable, not just write it to cache
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# installation rules, copy over binary to bin
install(TARGETS ttt DESTINATION bin)