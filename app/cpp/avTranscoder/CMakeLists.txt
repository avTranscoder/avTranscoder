### cpp/avTranscoder

include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(avtranscoder avTranscoder.cpp)
target_link_libraries(avtranscoder avtranscoder-shared)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/avtranscoder DESTINATION "bin/")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/av++.man DESTINATION "share/man/man1/")
