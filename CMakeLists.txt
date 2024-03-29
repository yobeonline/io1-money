cmake_minimum_required(VERSION 3.25)

option(IO1_WITH_TESTS
       "Add a target to build and run unit tests. Requires doctest." ON)

if(IO1_WITH_TESTS)
  list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

set(IO1_PROJECT_NAME "money")

project(
  io1-${IO1_PROJECT_NAME}
  VERSION 0.1.0
  DESCRIPTION
    "A class to store money amounts for personal accounting applications."
  HOMEPAGE_URL "https://github.com/yobeonline/io1-money"
  LANGUAGES CXX)

include(GNUInstallDirs)

add_library(${PROJECT_NAME} INTERFACE include/io1/money.hpp)
target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME
                                                 ${IO1_PROJECT_NAME})

add_library(io1::${IO1_PROJECT_NAME} ALIAS ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME})

install(
  EXPORT ${PROJECT_NAME}
  NAMESPACE io1::
  DESTINATION ${CMAKE_INSTALL_DATADIR}/io1/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/io1
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "io1ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/config.cmake.in" "io1Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/io1/cmake)

install(FILES "${PROJECT_BINARY_DIR}/io1ConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/io1Config.cmake"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/io1/cmake)

if(IO1_WITH_TESTS)
  include(CTest)

  find_package(doctest CONFIG REQUIRED)

  add_executable(test_${PROJECT_NAME} test/test_money.cpp test/tutorial.cpp)
  target_link_libraries(test_${PROJECT_NAME} PRIVATE io1::money
                                                     doctest::doctest)

  add_test(
    NAME test_${PROJECT_NAME}
    COMMAND test_${PROJECT_NAME} --reporters=junit
            --out=junit_test_${PROJECT_NAME}.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
