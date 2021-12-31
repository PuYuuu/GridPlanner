cmake_minimum_required(VERSION 3.16)
project(examples)


add_executable(main main.cpp)
target_link_libraries(main libGridPlanner ${SDL2_LIBRARIES} -lboost_program_options -lGL)
add_executable(test test.cpp)
target_link_libraries(test ${SDL2_LIBRARIES} -lGL)