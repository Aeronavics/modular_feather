cmake_minimum_required(VERSION 2.8)

project(tutorial_project)

find_library(UAVCAN_LIB uavcan REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11 -pthread")

# Change this to the absolute path to /platform_specific_components/linux/libuavcan/include
include_directories(/home/james/Documents/platform_specific_components/linux/libuavcan/include)

# Make sure to provide correct path to 'platform_linux.cpp'! See libuavcan tutorials for more info.
add_executable(configurator esc_set_params.cpp platform_linux.cpp)
target_link_libraries(configurator ${UAVCAN_LIB} rt)
