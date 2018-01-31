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

if(APPLE)
    set(CMAKE_CXX_COMPILER "llvm-g++")
    set(CMAKE_CXX_FLAGS "-std=c++11")
    #set(CMAKE_CXX_FLAGS "-std=c++11 -lpthread -Wno-deprecated -lc++ -stdlib=libc++ -DCRYPTOPP_DISABLE_ASM")
elseif(UNIX)
    set(CMAKE_CXX_COMPILER "g++-5")
    set(CMAKE_CXX_FLAGS "-std=c++11 -lpthread")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_DEBUG   "-Wall -Wextra -Wpedantic -g -ggdb")
    set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3")
    set(CMAKE_CXX_FLAGS_TEST    "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

set(RELEASE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/release")
set(DEBUG_DIRECTORY   "${CMAKE_SOURCE_DIR}/build/debug")
set(TEST_DIRECTORY    "${CMAKE_SOURCE_DIR}/build/test")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")
    
    file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/debug")

elseif(CMAKE_BUILD_TYPE MATCHES Release)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/release")
    
    file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RELEASE_DIRECTORY})

elseif(CMAKE_BUILD_TYPE MATCHES Test)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/build/test")
    file(REMOVE "${TEST_ROOT}/files/tmp/*")

    file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp" "${CMAKE_SOURCE_DIR}/test/*.cpp")
    list(REMOVE_ITEM SOURCES "${PROJECT_SOURCE_DIR}/main.cpp")

    set(TEST_ROOT "${CMAKE_SOURCE_DIR}/test/data")
    add_definitions( "-DTEST_ROOT=\"${TEST_ROOT}\"" )

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${TEST_DIRECTORY}")

endif()

add_executable(rechain ${SOURCES})

if(NOT CMAKE_SYSTEM_NAME MATCHES Windows)
    find_library(CRYPTOPP NAMES cryptopp crypto++ libcryptopp PATHS "/usr/lib/")
    target_link_libraries(rechain "${CRYPTOPP}")
endif()
