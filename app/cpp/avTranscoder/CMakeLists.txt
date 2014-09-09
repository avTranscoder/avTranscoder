### cpp/avTranscoder

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(avtranscoder avTranscoder.cpp)
target_link_libraries(avtranscoder avtranscoder-shared)

# Install app
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/av++.man DESTINATION "share/man/man1/")
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/avtranscoder
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
)
