cmake_minimum_required(VERSION 3.16)
project(GridPlanner)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE  "-std=c++11 -O3 -fopenmp -pthread")

if (CMAKE_HOST_WIN32)
    set(WINDOWS 1)
elseif(CMAKE_HOST_APPLE)
    set(MACOS 1)
elseif(CMAKE_HOST_UNIX)
    set(LINUX 1)
endif()

if(WINDOWS)
    link_directories("${PROJECT_SOURCE_DIR}")
    include_directories("${PROJECT_SOURCE_DIR}")
    set (GP_ROOT_DIR ${PROJECT_SOURCE_DIR})
    set (SDL2_LIBRARIES SDL2.dll)
    set (OPENGL_LIBRARIES opengl32)
elseif(LINUX)
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    set (OPENGL_LIBRARIES GL)
elseif(MACOS)
    
endif()

add_library(GridPlanner SHARED GridPlanner.cpp)
target_link_libraries(GridPlanner ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
set(GPLIB GridPlanner ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

add_executable(CreateMap CreateMap.cpp)
target_link_libraries(CreateMap ${GPLIB})

add_subdirectory(examples)
