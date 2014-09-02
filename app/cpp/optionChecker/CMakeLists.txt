### cpp/avOptionChecker

include_directories(${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})
# Add C++11 flag
set(CMAKE_CXX_FLAGS "-std=c++0x")
add_executable(avoptionchecker optionChecker.cpp)
target_link_libraries(avoptionchecker avtranscoder-shared)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/avoptionchecker DESTINATION "bin/")
