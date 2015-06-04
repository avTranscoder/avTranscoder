### cpp/avInfo

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
add_executable(avinfo avInfo.cpp)
set_target_properties(avinfo PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avinfo avtranscoder-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avinfo.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avinfo" "${CMAKE_CURRENT_BINARY_DIR}/avinfo-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
	OPTIONAL
)

install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/avinfo.man
	RENAME "avinfo.1"
	DESTINATION "share/man/man1/"
)
