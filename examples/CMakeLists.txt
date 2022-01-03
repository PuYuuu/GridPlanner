cmake_minimum_required(VERSION 3.16)
project(examples)


add_executable(main main.cpp)
target_link_libraries(main ${GBLIB})

add_executable(BFS BFS.cpp)
target_link_libraries(BFS ${GBLIB})

add_executable(DFS DFS.cpp)
target_link_libraries(DFS ${GBLIB})

add_executable(test test.cpp)
target_link_libraries(test ${SDL2_LIBRARIES} -lGL)