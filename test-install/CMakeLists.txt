cmake_minimum_required(VERSION 3.25)
project(test)

find_package(io1-money REQUIRED)
add_executable(test main.cpp)
target_link_libraries(test PRIVATE io1::money)
