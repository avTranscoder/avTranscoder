### cpp/avProcessor

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
add_executable(avaudiophasemeter avAudioPhaseMeter.cpp)
set_target_properties(avaudiophasemeter PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avaudiophasemeter avtranscoder-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avaudiophasemeter.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avaudiophasemeter" "${CMAKE_CURRENT_BINARY_DIR}/avaudiophasemeter-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
	OPTIONAL
)
