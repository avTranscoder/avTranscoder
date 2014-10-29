
# Create 'mediaio' shared lib
add_library(mediaIO-shared SHARED ${MEDIA_IO_SRC_FILES})
set_target_properties(mediaIO-shared PROPERTIES LINKER_LANGUAGE CXX)
if(WIN32)
	set_target_properties(mediaIO-shared PROPERTIES OUTPUT_NAME "mediaIO-${MEDIA_ENGINE_VERSION}")
else()
	set_target_properties(mediaIO-shared PROPERTIES OUTPUT_NAME mediaIO)
endif()
set_target_properties(mediaIO-shared PROPERTIES SOVERSION ${MEDIA_ENGINE_VERSION})
set_target_properties(mediaIO-shared PROPERTIES VERSION ${MEDIA_ENGINE_VERSION})
target_link_libraries(mediaIO-shared ${FFMPEG_LIBRARIES} mediaCore-shared mediaTransform-shared)

### Install mediaIO libs and include
install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	DESTINATION "include"
	FILES_MATCHING
	PATTERN "*.hpp"
	PATTERN "*.i"
)
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
install(
	DIRECTORY "${LIBRARY_OUTPUT_PATH}"
	DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

generate_python_binding("mediaIO")
generate_java_binding("mediaIO")
