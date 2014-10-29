### cpp/mediaChecker

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MediaEngineMacros)

# Build app
include_directories(${MEDIA_ENGINE_SRC_PATH} ${FFMPEG_INCLUDE_DIR})

# Add C++11 flag
if(NOT WIN32)
	set(CMAKE_CXX_FLAGS "-std=c++0x")
elseif(${MSVC_VERSION} LESS 1800)
	message("C++11 not supported, will not build mediaChecker app.")
	return()
endif()

add_executable(mediachecker mediaChecker.cpp)
set_target_properties(mediachecker PROPERTIES VERSION ${MEDIA_ENGINE_VERSION})
target_link_libraries(mediachecker mediaCore-shared mediaIO-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/mediachecker.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/mediachecker" "${CMAKE_CURRENT_BINARY_DIR}/mediachecker-${MEDIA_ENGINE_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
)

install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/mediachecker.man
	RENAME mediachecker.1
	DESTINATION "share/man/man1/"
)
