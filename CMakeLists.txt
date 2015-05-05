cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (TTT)

# outputs an executable
add_executable(ttt src/ttt.cpp)

# link the exe with the sound library
target_link_libraries(ttt -lasound)

#if user didnt set install dir, override it and write it to the cache -> Type and description necessary,
#overwrite variable, not just write it to cache
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# installation rules, copy over binary to bin
install(TARGETS ttt DESTINATION bin)