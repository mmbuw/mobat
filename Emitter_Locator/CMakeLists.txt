cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (TTT)

include_directories(external/glm0.9.6.3/)

# recorder
file(GLOB RECORDER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/recorder/*.cpp)
add_library(recorder STATIC ${RECORDER_SOURCES})
# populate both includedir and interfaceincludedir (include with targets linked with this)
target_include_directories(recorder PUBLIC include/recorder)
# requires libasound2-dev
target_link_libraries(recorder -lasound)

# signaltransformation
file(GLOB SIGNALTRANS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/signaltrans/*.cpp)
add_library(signaltrans STATIC ${SIGNALTRANS_SOURCES})
target_include_directories(signaltrans PUBLIC include/signaltrans)
# requires fftw3-dev and libsndfile1-dev
target_link_libraries(signaltrans -lfftw3_threads -lfftw3 -lm -lsndfile)

# locator
file(GLOB LOCATOR_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/locator/*.cpp)
add_library(locator STATIC ${LOCATOR_SOURCES})
target_include_directories(locator PUBLIC include/locator)
target_link_libraries(locator recorder signaltrans)

# visualization
file(GLOB RECORDER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/visualization/*.cpp)
add_library(visualization STATIC ${RECORDER_SOURCES})
target_include_directories(visualization PUBLIC include/visualization)
# requires libsfml-dev
target_link_libraries(visualization -lm -lsfml-graphics -lsfml-window -lsfml-system)


# output executables
add_executable(ttt src/visualization.cpp)
target_link_libraries(ttt recorder signaltrans locator visualization -lX11 )


#request C++11 if on unix, stop at all warnings but ignore specific warning in glm headers
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")
if(UNIX)
  add_definitions( -std=c++11 -Wall -Werror  -Wno-strict-aliasing -pthread)
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


# installation rules, copy over binaries to bin
install(TARGETS ttt DESTINATION bin)