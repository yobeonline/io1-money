cmake_minimum_required(VERSION 3.25)

project(test_io1-money_install LANGUAGES CXX)

find_package(io1 REQUIRED COMPONENTS money)

add_library(${PROJECT_NAME} test_install.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE io1::money)
