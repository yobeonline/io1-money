cmake_minimum_required(VERSION 3.18)

project(
  money
  VERSION 0.1.0
  DESCRIPTION
    "A class to store money amounts for personal accounting applications."
  HOMEPAGE_URL "https://github.com/yobeonline/money")

add_library(${PROJECT_NAME} INTERFACE) # include/money.hpp)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

option(WITH_TESTS "Add a target to build and run unit tests." ON)

if(WITH_TESTS)
  include(CTest)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

  find_package(Boost REQUIRED COMPONENTS unit_test_framework)
  add_executable(test_${PROJECT_NAME} test/test_money.cpp test/tutorial.cpp)
  target_link_libraries(test_${PROJECT_NAME} PRIVATE ${PROJECT_NAME}
                                                     Boost::unit_test_framework)

  add_test(
    NAME test_${PROJECT_NAME}
    COMMAND test_${PROJECT_NAME} --catch_system_error=yes --detect_memory_leaks
            --logger=JUNIT,all,junit_test_${PROJECT_NAME}.xml
    WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${PROJECT_NAME}>)
endif()
