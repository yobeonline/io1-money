cmake_minimum_required(VERSION 3.25)

option(WITH_TESTS "Add a target to build and run unit tests. Requires doctest."
       ON)

if(WITH_TESTS)
  list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

project(
  money
  VERSION 0.1.0
  DESCRIPTION
    "A class to store money amounts for personal accounting applications."
  HOMEPAGE_URL "https://github.com/yobeonline/money"
  LANGUAGES CXX)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE include/io1/money.hpp)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

add_library(io1::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} EXPORT money)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/io1 DESTINATION include)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${PROJECT_NAME}ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/config.cmake.in" "${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/io1/cmake)

install(
  EXPORT money
  NAMESPACE io1::
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/io1/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/io1/cmake)

if(WITH_TESTS)
  include(CTest)

  find_package(doctest CONFIG REQUIRED)

  add_executable(test_${PROJECT_NAME} test/test_money.cpp test/tutorial.cpp)
  target_link_libraries(test_${PROJECT_NAME} PRIVATE io1::${PROJECT_NAME}
                                                     doctest::doctest)

  add_test(
    NAME test_${PROJECT_NAME}
    COMMAND test_${PROJECT_NAME} --reporters=junit
            --out=junit_test_${PROJECT_NAME}.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
