### cpp/avOptionChecker

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
# Add C++11 flag
if(NOT WIN32)
	set(CMAKE_CXX_FLAGS "-std=c++0x")
elseif(${MSVC_VERSION} LESS 1800)
	message("C++11 not supported, will not build avOptionChecker app.")
	return()
endif()

add_executable(avoptionchecker optionChecker.cpp)
set_target_properties(avoptionchecker PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avoptionchecker avtranscoder-shared)

# Install app
if(WIN32)
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avoptionchecker.exe")
else()
	set(BINARY_FILES "${CMAKE_CURRENT_BINARY_DIR}/avoptionchecker" "${CMAKE_CURRENT_BINARY_DIR}/avoptionchecker-${AVTRANSCODER_VERSION}")
endif()

install(
	FILES ${BINARY_FILES}
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE
	DESTINATION "bin/"
        COMPONENT tools
)
