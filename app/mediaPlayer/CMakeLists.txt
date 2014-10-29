# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(MediaEngineMacros)

# Check OpenGL
find_package(OpenGL)
if(NOT OPENGL_FOUND)
	message("OpenGL not found, will not build mediaPlay app.")
	return()
endif()

# Check GLUT
find_package(GLUT)

if(NOT GLUT_Xi_LIBRARY OR NOT GLUT_Xmu_LIBRARY)
	set(GLUT_FOUND FALSE)
endif()

if(NOT GLUT_FOUND) 
	message("GLUT not found, will not build mediaPlay app.")
	return()
endif()

# Build app
include_directories(${MEDIA_ENGINE_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIR} )

file(GLOB MEDIAPLAY_SRC_FILES "*.cpp" "*.hpp")
add_executable(mediaplayer ${MEDIAPLAY_SRC_FILES})
set_target_properties(mediaplayer PROPERTIES VERSION ${MEDIA_ENGINE_VERSION})
target_link_libraries(mediaplayer mediaCore-shared mediaIO-shared mediaTransform-shared ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/mediaplayer.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/mediaplayer" "${CMAKE_CURRENT_BINARY_DIR}/mediaplayer-${MEDIA_ENGINE_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin"
)

install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/mediaplayer.man
	RENAME mediaplayer.1
	DESTINATION "share/man/man1"
)
