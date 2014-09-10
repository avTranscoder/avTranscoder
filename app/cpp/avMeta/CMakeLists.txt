### cpp/avMeta

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(avmeta avMeta.cpp)
set_target_properties(avmeta PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avmeta avtranscoder-shared)

# Install app
install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/avmeta" "${CMAKE_CURRENT_BINARY_DIR}/avmeta-${AVTRANSCODER_VERSION}"
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
)
install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/avmeta.man
	RENAME "avmeta.1"
	DESTINATION "share/man/man1/"
)
