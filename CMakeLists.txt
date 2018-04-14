cmake_minimum_required (VERSION 3.1)

# Set base project details
set (PROJECT_NAME "octopOS")
project (${PROJECT_NAME})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -g -Wall -O0 -fprofile-arcs -ftest-coverage")
if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
        EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
        if( ${ARCHITECTURE} STREQUAL "64Bit" )
                set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
        else()
        endif()
endif (CMAKE_BUILD_TYPE EQUAL "RELEASE")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

if(CMAKE_COMPILER_IS_GNUCXX)
    include(CodeCoverage)
    setup_target_for_coverage(${PROJECT_NAME}_coverage tests coverage)
endif()

# Create subscriber library
file(GLOB SubscriberSrc include/subscriber.h
                       include/tentacle.h
                       include/utility.h
                       src/subscriber.cpp
                       src/tentacle.cpp
                       src/utility.cpp)
add_library(Subscriber SHARED ${SubscriberSrc})
target_include_directories (Subscriber PUBLIC include src)

# Create publisher library
file(GLOB PublisherSrc include/publisher.h
                       include/tentacle.h
                       include/utility.h
                       src/tentacle.cpp
                       src/utility.cpp)
add_library(Publisher SHARED ${PublisherSrc})
target_include_directories (Publisher PUBLIC include src)

# Create octopOS library
file(GLOB OctopOSSrc include/octopos.h
                     include/tentacle.h
                     include/utility.h
                     src/octopos.cpp
                     src/tentacle.cpp
                     src/utility.cpp)
add_library(OctopOS SHARED ${OctopOSSrc})
target_include_directories (OctopOS PUBLIC include src)

# Get remaining source files
file(GLOB SOURCES "src/main.cpp")

#### Compile Main Executable ####
# setup base executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Check if pthreads.h is being used
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(PUBLIC ${PROJECT_NAME} "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}" ${Boost_LIBRARIES} Subscriber OctopOS Publisher)
endif()

#### Compile Unit Tests ####
enable_testing()

get_filename_component(TEST_EXECUTABLE_NAME tests/test_octopOS.cpp NAME_WE)

# Get Boost libraries
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

add_executable(${TEST_EXECUTABLE_NAME} tests/test_octopOS.cpp)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(PUBLIC ${PROJECT_NAME} "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
target_link_libraries(${TEST_EXECUTABLE_NAME} "${CMAKE_THREAD_LIBS_INIT}"
    ${Boost_LIBRARIES} OctopOS Publisher Subscriber)
endif()

file(READ tests/test_octopOS.cpp SOURCE_FILE_CONTENTS)
string(REGEX MATCHALL "BOOST_AUTO_TEST_CASE\\( *([A-Za-z_0-9]+) *\\)"
       FOUND_TESTS ${SOURCE_FILE_CONTENTS})

foreach(HIT ${FOUND_TESTS})
    string(REGEX REPLACE ".*\\( *([A-Za-z_0-9]+) *\\).*" "\\1" TEST_NAME ${HIT})

    add_test(NAME "${TEST_EXECUTABLE_NAME}.${TEST_NAME}"
        COMMAND ${TEST_EXECUTABLE_NAME}
        --run_test=${TEST_NAME} --catch_system_error=yes)
endforeach()
