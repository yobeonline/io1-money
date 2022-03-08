cmake_minimum_required(VERSION 3.18)

project(money)

set(CMAKE_C_FLAGS_COVERAGE "${CMAKE_C_FLAGS_DEBUG} --coverage")
set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
set(CMAKE_STATIC_LINKER_FLAGS_COVERAGE "${CMAKE_STATIC_LINKER_FLAGS_DEBUG}")
set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
list(APPEND CMAKE_CONFIGURATION_TYPES Coverage)

include(CTest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

add_library(${PROJECT_NAME} INTERFACE) # include/money.hpp)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if(BUILD_TESTING)
  find_package(Boost REQUIRED)
  add_executable(test_${PROJECT_NAME} test/test_money.cpp test/tutorial.cpp)
  target_link_libraries(test_${PROJECT_NAME} PRIVATE ${PROJECT_NAME}
                                                     Boost::boost)

  add_test(
    NAME test_${PROJECT_NAME}
    COMMAND test_${PROJECT_NAME} --catch_system_error=yes --detect_memory_leaks
            --logger=JUNIT,all,junit_test_${PROJECT_NAME}.xml
    WORKING_DIRECTORY $<TARGET_FILE_DIR:test_${PROJECT_NAME}>)
endif()
