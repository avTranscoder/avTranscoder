cmake_minimum_required(VERSION 2.8.11) 

project(AvTranscoder)

# Set AvTranscoder versions
set(AVTRANSCODER_VERSION_MAJOR "0")
set(AVTRANSCODER_VERSION_MINOR "5")
set(AVTRANSCODER_VERSION_MICRO "5")
set(AVTRANSCODER_VERSION ${AVTRANSCODER_VERSION_MAJOR}.${AVTRANSCODER_VERSION_MINOR}.${AVTRANSCODER_VERSION_MICRO})

# Define AvTranscoder versions
add_definitions(-DAVTRANSCODER_VERSION_MAJOR=${AVTRANSCODER_VERSION_MAJOR})
add_definitions(-DAVTRANSCODER_VERSION_MINOR=${AVTRANSCODER_VERSION_MINOR})
add_definitions(-DAVTRANSCODER_VERSION_MICRO=${AVTRANSCODER_VERSION_MICRO})

# Define AvTranscoder default path to profiles
add_definitions(-DAVTRANSCODER_DEFAULT_AVPROFILES="${CMAKE_INSTALL_PREFIX}/share/ressource")

# Diplay commands being ran by CMake
set(CMAKE_VERBOSE_MAKEFILE OFF)

# CPP flags on debug / release mode
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fPIC -pg -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC -O3")
endif()

# CPP flag to create code coverage report
if(AVTRANSCODER_COVERAGE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

add_subdirectory(src)

if(AVTRANSCODER_DISABLE_APPS)
	message("Apps disabled, will not build applications.")
else()
	add_subdirectory(app)
endif()
