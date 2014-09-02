### cpp/avProcessor

include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(avprocessor genericProcessor.cpp)
target_link_libraries(avprocessor avtranscoder-shared)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/avprocessor DESTINATION "bin/")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/avprocessor.man DESTINATION "share/man/man1/")
