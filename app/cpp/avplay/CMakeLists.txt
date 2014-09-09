### cpp/avplay

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

file(GLOB AVPLAY_SRC_FILES "*.cpp" "*.hpp")
add_executable(avplay ${AVPLAY_SRC_FILES})
target_link_libraries(avplay avtranscoder-shared ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

# Install app
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/avplay
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
)
install(
	FILES ${CMAKE_CURRENT_SOURCE_DIR}/avplayer.man
	RENAME avplayer.1
	DESTINATION "share/man/man1/"
)
