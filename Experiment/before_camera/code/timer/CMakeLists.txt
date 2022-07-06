cmake_minimum_required (VERSION 3.9)

# Project name
project(SC19 LANGUAGES CXX)

# Enable test
include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

# Find pthread package
find_package(Threads REQUIRED)

# add the binary tree to the search path for include files
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/../../cpp-taskflow)
include_directories(${PROJECT_SOURCE_DIR}/tbb-2019_U2/include)


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

link_directories(${PROJECT_SOURCE_DIR}/tbb-2019_U2/build/linux_intel64_gcc_cc8_libc2.27_kernel4.18.0_release/)
#add_library(tbblib SHARED ${PROJECT_SOURCE_DIR}/tbb-2019_U2/build/linux_intel64_gcc_cc8_libc2.27_kernel4.18.0_release/libtbb.so) 

add_executable(ot
  ${PROJECT_SOURCE_DIR}/main.cpp
  ${PROJECT_SOURCE_DIR}/taskflow.cpp
  ${PROJECT_SOURCE_DIR}/tbb.cpp
)
target_link_libraries(
  ot
  Threads::Threads tbb stdc++fs
)
