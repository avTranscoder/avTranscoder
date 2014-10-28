
# Create 'mediacore' shared lib
add_library(mediaCore-shared SHARED ${MEDIA_CORE_SRC_FILES})
set_target_properties(mediaCore-shared PROPERTIES LINKER_LANGUAGE CXX)
if(WIN32)
	set_target_properties(mediaCore-shared PROPERTIES OUTPUT_NAME "mediaCore-${MEDIA_MANAGER_VERSION}")
else()
	set_target_properties(mediaCore-shared PROPERTIES OUTPUT_NAME mediaCore)
endif()
set_target_properties(mediaCore-shared PROPERTIES SOVERSION ${MEDIA_MANAGER_VERSION})
set_target_properties(mediaCore-shared PROPERTIES VERSION ${MEDIA_MANAGER_VERSION})
target_link_libraries(mediaCore-shared ${FFMPEG_LIBRARIES})

### Install mediacore libs and include
install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	DESTINATION "include"
	FILES_MATCHING
	PATTERN "*.hpp"
	PATTERN "*.i"
	PATTERN "profile/Avi.hpp" EXCLUDE
	PATTERN "profile/DNxHD.hpp" EXCLUDE
	PATTERN "profile/Mkv.hpp" EXCLUDE
	PATTERN "profile/Wave.hpp" EXCLUDE
	PATTERN "profile/XdCamHd422.hpp" EXCLUDE
)
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
install(
	DIRECTORY "${LIBRARY_OUTPUT_PATH}"
	DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

generate_python_binding("mediaCore")
generate_java_binding("mediaCore")
