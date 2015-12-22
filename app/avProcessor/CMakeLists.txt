### cpp/avProcessor

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
add_executable(avprocessor avProcessor.cpp)
set_target_properties(avprocessor PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avprocessor avtranscoder-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avprocessor.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avprocessor" "${CMAKE_CURRENT_BINARY_DIR}/avprocessor-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
	OPTIONAL
)
