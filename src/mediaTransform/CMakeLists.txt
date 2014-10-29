
# Create 'mediaTransform' shared lib
add_library(mediaTransform-shared SHARED ${MEDIA_TRANSFORM_SRC_FILES})
set_target_properties(mediaTransform-shared PROPERTIES LINKER_LANGUAGE CXX)
if(WIN32)
	set_target_properties(mediaTransform-shared PROPERTIES OUTPUT_NAME "mediaTransform-${MEDIA_ENGINE_VERSION}")
else()
	set_target_properties(mediaTransform-shared PROPERTIES OUTPUT_NAME mediaTransform)
endif()
set_target_properties(mediaTransform-shared PROPERTIES SOVERSION ${MEDIA_ENGINE_VERSION})
set_target_properties(mediaTransform-shared PROPERTIES VERSION ${MEDIA_ENGINE_VERSION})
target_link_libraries(mediaTransform-shared ${FFMPEG_LIBRARIES} mediaCore-shared)


### Install mediaTransform libs and include
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

generate_python_binding("mediaTransform")
generate_java_binding("mediaTransform")
