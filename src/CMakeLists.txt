# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MediaManagerMacros)

find_package(
	FFmpeg
	COMPONENTS
		avcodec
		avformat
		avutil
		swscale
		swresample
		avresample
)

find_package(SWIG)

# Check if FFmpeg or libav dependency
if(EXISTS "${FFMPEG_INCLUDE_DIR}/libavresample/avresample.h")
	add_definitions(-DAV_RESAMPLE_LIBRARY)
else()
	add_definitions(-DFF_RESAMPLE_LIBRARY)
endif()

# Include mediaManager and FFmpeg/Libav
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${FFMPEG_INCLUDE_DIR})

add_subdirectory(mediaCore)
add_subdirectory(mediaIO)
add_subdirectory(mediaTransform)
