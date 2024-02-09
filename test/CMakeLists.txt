cmake_minimum_required(VERSION 3.25)

project(test_io1-money LANGUAGES CXX)

include(CTest)

find_package(doctest REQUIRED)
if (NOT TARGET io1::money)
  find_package(io1_money REQUIRED)
endif()

add_executable(${PROJECT_NAME} test/test_money.cpp test/tutorial.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE io1::money
											 doctest::doctest)
add_test(
NAME ${PROJECT_NAME}
COMMAND ${PROJECT_NAME} --reporters=junit
		--out=junit_${PROJECT_NAME}.xml
WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
