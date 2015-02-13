### cpp/avProcessor

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
add_executable(avthumbnail avThumbnail.cpp)
set_target_properties(avthumbnail PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avthumbnail avtranscoder-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avthumbnail.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avthumbnail" "${CMAKE_CURRENT_BINARY_DIR}/avthumbnail-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
)
