### cpp/customEncoder

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
add_executable(custom-encoder customEncoder.cpp)
set_target_properties(custom-encoder PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(custom-encoder avtranscoder-shared)

# Install app
if(WIN32)
	set_target_properties(custom-encoder PROPERTIES OUTPUT_NAME "custom-encoder-${AVTRANSCODER_VERSION}")
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/custom-encoder.exe" "${CMAKE_CURRENT_BINARY_DIR}/custom-encoder-${AVTRANSCODER_VERSION}.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/custom-encoder" "${CMAKE_CURRENT_BINARY_DIR}/custom-encoder-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
	OPTIONAL
)
