### cpp/avplayer

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Check OpenGL
find_package(OpenGL)
if(NOT OPENGL_FOUND)
	message("OpenGL not found, will not build avplayer app.")
	return()
endif()

# Check GLUT
find_package(GLUT)

if(NOT GLUT_Xi_LIBRARY OR NOT GLUT_Xmu_LIBRARY)
	set(GLUT_FOUND FALSE)
endif()

if(NOT GLUT_FOUND) 
	message("GLUT not found, will not build avplay app.")
	return()
endif()

# Build app
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIR} )

file(GLOB AVPLAY_SRC_FILES "*.cpp" "*.hpp")
add_executable(avplayer ${AVPLAY_SRC_FILES})
set_target_properties(avplayer PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avplayer avtranscoder-shared ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

# Install app
if(WIN32)
	set_target_properties(avplayer PROPERTIES OUTPUT_NAME "avplayer-${AVTRANSCODER_VERSION}")
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avplayer.exe" "${CMAKE_CURRENT_BINARY_DIR}/avplayer-${AVTRANSCODER_VERSION}.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avplayer" "${CMAKE_CURRENT_BINARY_DIR}/avplayer-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
	OPTIONAL
)
