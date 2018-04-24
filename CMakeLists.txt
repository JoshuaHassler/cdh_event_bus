cmake_minimum_required (VERSION 3.1)

# Includes for installing
include(GNUInstallDirs)

# Set base project details
set (PROJECT_NAME "octopOS")
project (${PROJECT_NAME} VERSION 0.6.0 DESCRIPTION "data bus libraries")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -O0 -fprofile-arcs -ftest-coverage")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

if(CMAKE_COMPILER_IS_GNUCXX)
    include(CodeCoverage)
    setup_target_for_coverage(${PROJECT_NAME}_coverage tests coverage)
endif()

# Create utility library
file(GLOB UtilitySrc include/utility.h
                     src/utility.cpp)
add_library(Utility SHARED ${UtilitySrc})
target_include_directories (Utility PUBLIC include src)
set_target_properties(Utility PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(Utility PROPERTIES SOVERSION 0)
set_target_properties(Utility PROPERTIES PUBLIC_HEADER include/utility.h)

# Create subscriber library
file(GLOB TentacleSrc include/subscriber.h
                       include/tentacle.h
                       include/utility.h
                       src/tentacle.cpp)
add_library(Tentacle SHARED ${TentacleSrc})
target_include_directories (Tentacle PUBLIC include src)
set_target_properties(Tentacle PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(Tentacle PROPERTIES SOVERSION 0)
set_target_properties(Tentacle PROPERTIES PUBLIC_HEADER include/tentacle.h)

file(GLOB SubscriberSrc include/subscriber.h
                       include/tentacle.h
                       include/utility.h
                       src/subscriber.cpp
                       src/tentacle.cpp)
add_library(Subscriber SHARED ${SubscriberSrc})
target_include_directories (Subscriber PUBLIC include src)
set_target_properties(Subscriber PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(Subscriber PROPERTIES SOVERSION 0)
set_target_properties(Subscriber PROPERTIES PUBLIC_HEADER include/subscriber.h)

# Create publisher library
file(GLOB PublisherSrc include/publisher.h
                       include/tentacle.h
                       include/utility.h
                       src/tentacle.cpp)
add_library(Publisher SHARED ${PublisherSrc})
target_include_directories (Publisher PUBLIC include src)
set_target_properties(Publisher PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(Publisher PROPERTIES SOVERSION 0)
set_target_properties(Publisher PROPERTIES PUBLIC_HEADER include/publisher.h)

# Create octopOS library
file(GLOB OctopOSSrc include/octopos.h
                     include/tentacle.h
                     include/utility.h
                     src/octopos.cpp
                     src/tentacle.cpp)
add_library(OctopOS SHARED ${OctopOSSrc})
target_include_directories (OctopOS PUBLIC include src)
set_target_properties(OctopOS PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(OctopOS PROPERTIES SOVERSION 0)
set_target_properties(OctopOS PROPERTIES PUBLIC_HEADER include/octopos.h)

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
    ${Boost_LIBRARIES} OctopOS Publisher Subscriber Utility)
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

#### Install OctopOS as a library ####
install(TARGETS Subscriber Publisher Tentacle OctopOS Utility
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/OctopOS"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/OctopOS")
