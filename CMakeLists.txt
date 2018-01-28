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

# find the source files with wildcards
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

# set flags for compilation and linking
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread -Wall -Wextra -Wpedantic -g -ggdb")

add_executable(rechain ${SOURCES})
target_link_libraries(rechain -lcrypto++)
