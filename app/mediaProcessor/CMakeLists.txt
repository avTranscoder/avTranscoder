### cpp/avProcessor

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MediaEngineMacros)

# Build app
include_directories(${MEDIA_ENGINE_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(mediaprocessor mediaProcessor.cpp)
set_target_properties(mediaprocessor PROPERTIES VERSION ${MEDIA_ENGINE_VERSION})
target_link_libraries(mediaprocessor mediaCore-shared mediaIO-shared mediaTransform-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/mediaprocessor.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/mediaprocessor" "${CMAKE_CURRENT_BINARY_DIR}/mediaprocessor-${MEDIA_ENGINE_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin"
)

install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/mediaprocessor.man
	RENAME mediaprocessor.1
	DESTINATION "share/man/man1"
)
