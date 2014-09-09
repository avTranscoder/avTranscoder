### cpp/avMeta

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(avmeta avMeta.cpp)
target_link_libraries(avmeta avtranscoder-shared)

# Install app
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/avmeta DESTINATION "bin/")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/avmeta.man DESTINATION "share/man/man1/")
