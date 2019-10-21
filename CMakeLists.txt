cmake_minimum_required(VERSION 2.8.11) 

project(AvTranscoder)

# All libraries will be put in INSTALL_PREFIX/lib
# RPATH of host points INSTALL_PREFIX/lib
# see: http://www.cmake.org/Wiki/CMake_RPATH_handling
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# Define AvTranscoder default path to profiles
add_definitions(-DAVTRANSCODER_DEFAULT_AVPROFILES="${CMAKE_INSTALL_PREFIX}/share/avprofiles")

# Diplay commands being ran by CMake
set(CMAKE_VERBOSE_MAKEFILE OFF)

# The install rule does not depend on all, i.e. everything will not be built before installing
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# CPP flags on debug / release mode
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Ox")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fPIC -g")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -fPIC -O3")
endif()

# CPP flag to create code coverage report
if(AVTRANSCODER_COVERAGE)
	message("Add coverage build option.")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

# Build library
add_subdirectory(src)

# Build apps
if(AVTRANSCODER_DISABLE_APPS)
	message("Apps disabled, will not build applications.")
else()
	add_subdirectory(app)
endif()
