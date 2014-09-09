cmake_minimum_required(VERSION 2.8) 

project(AvTranscoder)

# Define AvTranscoder versionS
add_definitions(-DAVTRANSCODER_VERSION_MAJOR=0)
add_definitions(-DAVTRANSCODER_VERSION_MINOR=2)
add_definitions(-DAVTRANSCODER_VERSION_MICRO=1)
set(AVTRANSCODER_VERSION "0.2.1")

# Diplay commands being ran by CMake
set(CMAKE_VERBOSE_MAKEFILE ON)

# CPP flags on debug / release mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fPIC -pg -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3")

add_subdirectory(src)
add_subdirectory(app)
