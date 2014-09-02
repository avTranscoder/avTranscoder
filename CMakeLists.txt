cmake_minimum_required(VERSION 2.8) 

project(AvTranscoder)

# AvTranscoder version
set(AVTRANSCODER_VERSION_MAJOR 0)
set(AVTRANSCODER_VERSION_MINOR 2)
set(AVTRANSCODER_VERSION_MICRO 1)
set(AVTRANSCODER_VERSION ${AVTRANSCODER_VERSION_MAJOR}.${AVTRANSCODER_VERSION_MINOR}.${AVTRANSCODER_VERSION_MICRO})

# Diplay commands being ran by CMake
set(CMAKE_VERBOSE_MAKEFILE ON)

# Choose compiler
set(CMAKE_CXX_COMPILER "g++")

# CPP flags on debug / release mode
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fPIC -pg -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3")

add_subdirectory(src)
add_subdirectory(app)
