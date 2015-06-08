cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (TTT)

#add include folder

# recorder
file(GLOB RECORDER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/recorder/*.cpp)
add_library(recorder STATIC ${RECORDER_SOURCES})
target_include_directories(recorder PUBLIC include/recorder)
# requires libasound2-dev
target_link_libraries(recorder -lasound)

# visualization
file(GLOB RECORDER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/visualization/*.cpp)
add_library(visualization STATIC ${RECORDER_SOURCES})
target_include_directories(visualization PUBLIC include/visualization )
# requires libsfml-dev
target_link_libraries(recorder -lasound signaltrans -lfftw3_threads -lfftw3 -lm visualization -lsfml-graphics -lsfml-window -lsfml-system)

# signaltransformation
file(GLOB SIGNALTRANS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/signaltrans/*.cpp)
add_library(signaltrans STATIC ${SIGNALTRANS_SOURCES})
target_include_directories(signaltrans PUBLIC include/signaltrans)
# requires fftw3-dev and libsndfile1-dev
target_link_libraries(signaltrans -lfftw3_threads -lfftw3 -lm -lsndfile)

# output executable
add_executable(ttt src/ttt.cpp)
target_link_libraries(ttt recorder -lasound)

add_executable(sigtrans src/sigtrans.cpp)
target_include_directories(sigtrans PUBLIC include/signaltrans)
target_link_libraries(sigtrans signaltrans)

add_executable(visualizer src/visualization.cpp)
target_include_directories(visualizer PUBLIC include/recorder include/signaltrans include/visualization)
target_link_libraries(recorder signaltrans visualization)

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
