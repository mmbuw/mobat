cmake_minimum_required (VERSION 2.8)
# allows access to environment variables with the name 
project (TTT)

include_directories(external/glm0.9.6.3/)

option(ODROID_BUILD "Build visualisation for odroid/raspberry" OFF)

#testing
file(GLOB TESTING_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/testing/*.cpp)
add_library(testing STATIC ${TESTING_SOURCES})
# populate both includedir and interfaceincludedir (include with targets linked with this)
target_include_directories(testing PUBLIC include/testing)
# requires boostlib
target_link_libraries(testing -lboost_filesystem -lboost_system visualization)

# udp_reciever
file(GLOB NETWORKING_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} networking/*.cpp)
add_library(networking STATIC ${NETWORKING_SOURCES})
# populate both includedir and interfaceincludedir (include with targets linked with this)
target_include_directories(networking PUBLIC networking)

# udp_reciever
file(GLOB UDP_RECEIVER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/udp_receiver/*.cpp)
add_library(udp_receiver STATIC ${UDP_RECEIVER_SOURCES})
# populate both includedir and interfaceincludedir (include with targets linked with this)
target_include_directories(udp_receiver PUBLIC include/udp_receiver)
# requires boostlib
target_link_libraries(udp_receiver visualization)

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
target_include_directories(signaltrans PUBLIC include/signaltrans include/locator)
# requires fftw3-dev
target_link_libraries(signaltrans -lfftw3_threads -lfftw3 -lm visualization)

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
if(ODROID_BUILD)
  target_link_libraries(visualization -lxcb -lxcb-randr -lxcb-icccm -lxcb-image -ludev)
endif()

# mobat_server
file(GLOB MOBAT_SERVER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/mobat_server/*.cpp)
add_library(mobat_server STATIC ${MOBAT_SERVER_SOURCES})
target_include_directories(mobat_server PUBLIC include/mobat_server)
target_link_libraries(mobat_server locator testing networking -lm -lsfml-graphics -lsfml-window -lsfml-system)
if(ODROID_BUILD)
  target_link_libraries(mobat_server -lxcb -lxcb-randr -lxcb-icccm -lxcb-image -ludev)
endif()

# output executables
add_executable(ttt src/visualization.cpp)
target_link_libraries(ttt recorder signaltrans locator visualization testing udp_receiver networking -lpthread)
if(ODROID_BUILD)
  target_link_libraries(ttt -lX11 -lX11-xcb)
endif()

add_executable(mobat_localizer src/mobat_localizer.cpp)
target_link_libraries(mobat_localizer mobat_server recorder signaltrans locator visualization testing networking -lpthread)
if(ODROID_BUILD)
  target_link_libraries(mobat_localizer -lX11 -lX11-xcb)
endif()

# output executables
add_executable(table_plot src/table_plot/svg_plot_generator.cpp)
#target_link_libraries(ttt recorder signaltrans locator visualization testing -lpthread)
#if(ODROID_BUILD)
#  target_link_libraries(ttt -lX11 -lX11-xcb)
#endif()

#request C++11 if on unix, stop at all warnings but ignore specific warning in glm headers
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
if(UNIX)
  add_definitions( -std=c++11 -Wall -Werror -Wno-strict-aliasing)
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
