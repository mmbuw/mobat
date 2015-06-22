cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (TTT)

include_directories(external/glm0.9.6.3/)

# locator
file(GLOB LOCATOR_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/locator/*.cpp)
add_library(locator STATIC ${LOCATOR_SOURCES})
target_include_directories(locator PUBLIC include/locator)
target_include_directories(locator PUBLIC include/recorder)
target_include_directories(locator PUBLIC include/signaltrans)


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
target_link_libraries(visualization -lm -lsfml-graphics -lsfml-window -lsfml-system)

# signaltransformation
file(GLOB SIGNALTRANS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/signaltrans/*.cpp)
add_library(signaltrans STATIC ${SIGNALTRANS_SOURCES})
target_include_directories(signaltrans PUBLIC include/signaltrans)
# requires fftw3-dev and libsndfile1-dev
target_link_libraries(signaltrans -lfftw3_threads -lfftw3 -lm -lsndfile)

# output executables
add_executable(recorder_example src/ttt.cpp)
target_link_libraries(recorder_example recorder -lasound)

add_executable(sigtrans_example src/sigtrans.cpp)
target_include_directories(sigtrans_example PUBLIC include/signaltrans)
target_link_libraries(sigtrans_example signaltrans)

add_executable(visualizer_example src/visualization.cpp)
target_include_directories(visualizer_example PUBLIC include/recorder include/signaltrans include/locator include/visualization)
target_link_libraries(visualizer_example recorder signaltrans locator visualization -lX11 )

add_executable(locator_example src/locate.cpp)
target_link_libraries(locator_example locator)



#request C++11 if on unix, generate debug symbols and output all errors
if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Werror -pthread")
endif(UNIX)

#if user didnt set install dir, override it and write it to the cache -> Type and description necessary,
#overwrite variable, not just write it to cache
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install" CACHE STRING "Install path prefix, prepended onto install directories." FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# installation rules, copy over binaries to bin
install(TARGETS recorder_example DESTINATION bin)
install(TARGETS sigtrans_example DESTINATION bin)
install(TARGETS visualizer_example DESTINATION bin)
