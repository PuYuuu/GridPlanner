cmake_minimum_required(VERSION 3.16)
project(GridPlanner)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE  "-std=c++11 -O3 -fopenmp -pthread")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(libGridPlanner SHARED GridPlanner.cpp)
<<<<<<< HEAD
set(GBLIB libGridPlanner ${SDL2_LIBRARIES} -lboost_program_options -lGL)
=======
>>>>>>> 9adccb42f927415890ec6c84a72aaf36f832b378

add_subdirectory(examples)