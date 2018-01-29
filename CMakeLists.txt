# cmake version and project name
cmake_minimum_required(VERSION 3.5.1)
project(rechain)

set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(PROJECT_BINARY_DIR "${CMAKE_SOURCE_DIR}/bin")

# extract the version string from the last git tag and define it for the project
execute_process(COMMAND -E git describe --abbrev=0 --tags RESULT_VARIABLE REVERSION)
add_definitions(-DRECHAIN_VERSION="v0.1.0")

# set the include directories
include_directories("${CMAKE_SOURCE_DIR}/inc/" "${CMAKE_SOURCE_DIR}/inc/dependencies/")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")
    
    # find the source files with wildcards
    file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

    # set flags for compilation and linking
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread -Wall -Wextra -Wpedantic -g -ggdb")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")

elseif(CMAKE_BUILD_TYPE MATCHES Release)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/release")
    
    # find the source files with wildcards
    file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

    # set flags for compilation and linking
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -std=c++11 -lpthread -lcrypto++ -O3")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/release")

elseif(CMAKE_BUILD_TYPE MATCHES Test)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/test")
    
    set(TEST_FILES "${CMAKE_SOURCE_DIR}/test/data")

    # find the source files with wildcards
    file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp" "${CMAKE_SOURCE_DIR}/test/*.cpp")
    list(REMOVE_ITEM SOURCES "${PROJECT_SOURCE_DIR}/main.cpp")

    # set flags for compilation and linking
    set(CXX_VARIABLES "-DTEST_FILES=${TEST_FILES}")
    set(CXX_FLAGS  "-std=c++11 -lcrypto++ -lpthread -Wall -Wextra -Wpedantic -g -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_VARIABLES} ${CXX_FLAGS}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/test")

endif()

add_executable(rechain ${SOURCES})
target_link_libraries(rechain -lcrypto++)
