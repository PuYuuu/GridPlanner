cmake_minimum_required(VERSION 3.16)
project(examples)


add_executable(main main.cpp)
target_link_libraries(main ${GPLIB})

add_executable(BFS BFS.cpp)
target_link_libraries(BFS ${GPLIB})

add_executable(DFS DFS.cpp)
target_link_libraries(DFS ${GPLIB})

add_executable(test test.cpp)
target_link_libraries(test ${GPLIB})

if(WINDOWS)
    # Copy DLL files to output directory
    add_custom_command(TARGET BFS POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GP_ROOT_DIR}/SDL2.dll"              
    $<TARGET_FILE_DIR:BFS>)

    add_custom_command(TARGET BFS POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GP_ROOT_DIR}/build/libGridPlanner.dll"              
    $<TARGET_FILE_DIR:BFS>)
endif()
