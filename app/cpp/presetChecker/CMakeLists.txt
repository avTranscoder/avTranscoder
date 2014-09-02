### cpp/avPresetChecker

include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
# Add C++11 flag
set(CMAKE_CXX_FLAGS "-std=c++0x")
add_executable(avpresetchecker presetChecker.cpp)
target_link_libraries(avpresetchecker avtranscoder-shared)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/avpresetchecker DESTINATION "bin/")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/avprofiles.man DESTINATION "share/man/man1/")
