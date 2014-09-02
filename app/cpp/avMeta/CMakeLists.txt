### cpp/avMeta

include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
add_executable(avmeta avMeta.cpp)
target_link_libraries(avmeta avtranscoder-shared)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/avmeta DESTINATION "bin/")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/avmeta.man DESTINATION "share/man/man1/")
