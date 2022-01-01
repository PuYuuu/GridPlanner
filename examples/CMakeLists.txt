cmake_minimum_required(VERSION 3.16)
project(examples)


add_executable(main main.cpp)
<<<<<<< HEAD
target_link_libraries(main ${GBLIB})

add_executable(BFS BFS.cpp)
target_link_libraries(BFS ${GBLIB})

=======
target_link_libraries(main libGridPlanner ${SDL2_LIBRARIES} -lboost_program_options -lGL)
>>>>>>> 9adccb42f927415890ec6c84a72aaf36f832b378
add_executable(test test.cpp)
target_link_libraries(test ${SDL2_LIBRARIES} -lGL)