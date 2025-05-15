cmake_minimum_required(VERSION 3.31)
project(fb2png CXX)
add_executable(fb2png fb2png_neon.cpp main.cpp)
target_link_libraries(fb2png png)