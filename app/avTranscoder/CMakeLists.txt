### cpp/avTranscoder

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(avtranscoder avTranscoder.cpp)
set_target_properties(avtranscoder PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avtranscoder avtranscoder-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avtranscoder.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avtranscoder" "${CMAKE_CURRENT_BINARY_DIR}/avtranscoder-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
        COMPONENT tools
)

install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/av++.man
	RENAME av++.1
	DESTINATION "share/man/man1/"
        COMPONENT tools
)
