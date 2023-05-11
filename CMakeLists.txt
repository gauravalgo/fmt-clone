cmake_minimum_required(VERSION 3.0)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "choose the type of build , None(CMAKE_CXX_FLAGS Or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel .")
endif()
project(FORMAT)
#set the default CMAKE_BUILD_TYPE to Release
#this should be done before the project command since the latter can set
#CMAKE_BUILD_TYPE itself(it does so for nmake).

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

#Speed test
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tinyformat/tinyformat_test.cpp)
    add_executable(tinyformat_speed_test tinyformat/tinyformat_test.cpp)
    target_link_libraries(tinyformat_speed_test format)    
    set_target_properties(tinyformat_speed_test PROPERTIES COMPILE_DEFINITIONS "SPEED_TEST;HAVE_FORMAT")
    add_custom_target(speed_test 
         COMMAND @echo running speed tests ...
         COMMAND @echo printf timings:
         COMMAND @time -p ./tinyformat_speed_test printf > /dev/null
         COMMAND @echo iostreams timings:
         COMMAND @time -p ./tinyformat_speed_test iostreams > /dev/null
         COMMAND @echo tinyformat timings:
         COMMAND @time -p ./tinyformat_speed_test tinyformat > /dev/null 
         COMMAND @echo boost timings:
         COMMAND @time -p ./tinyformat_speed_test boost > /dev/null)
endif()         
    