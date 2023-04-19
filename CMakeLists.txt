cmake_minimum_required(VERSION 3.0)
project(format)
set( SOURCES format.h format.cc)
add_library(format STATIC ${SOURCES})

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest/CMakeLists.txt)
    #add_subdirectory(googletest/gtest)
    #include_directories(googletest/googletest/include)
    #
    add_subdirectory(./googletest)

    #include(gtest)
    enable_testing()
    include_directories(googletest/googletest/include ${gtest_SOURCE_DIR})
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/googletest/googletest)
    add_executable(format_test format_test.cc ./googletest/googletest/src/gtest_main.cc)
    target_link_libraries(format_test format gtest)
    add_test(format_test format_test)
endif()