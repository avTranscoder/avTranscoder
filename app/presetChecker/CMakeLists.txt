### cpp/avPresetChecker

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
# Add C++11 flag
if(NOT WIN32)
	set(CMAKE_CXX_FLAGS "-std=c++0x")
elseif(${MSVC_VERSION} LESS 1800)
	message("C++11 not supported, will not build avPresetChecker app.")
	return()
endif()

add_executable(avpresetchecker presetChecker.cpp)
set_target_properties(avpresetchecker PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avpresetchecker avtranscoder-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avpresetchecker.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avpresetchecker" "${CMAKE_CURRENT_BINARY_DIR}/avpresetchecker-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
)

install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/avprofiles.man
	RENAME avprofiles.1
	DESTINATION "share/man/man1/"
)
