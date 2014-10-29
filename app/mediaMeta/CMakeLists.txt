# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MediaEngineMacros)

# Build app
include_directories(${MEDIA_ENGINE_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(mediameta mediaMeta.cpp)
set_target_properties(mediameta PROPERTIES VERSION ${MEDIA_ENGINE_VERSION})
target_link_libraries(mediameta mediaCore-shared mediaIO-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/mediameta.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/mediameta" "${CMAKE_CURRENT_BINARY_DIR}/mediameta-${MEDIA_ENGINE_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin"
)

install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/mediameta.man
	RENAME "mediameta.1"
	DESTINATION "share/man/man1"
)
