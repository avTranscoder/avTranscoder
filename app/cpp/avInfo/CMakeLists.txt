### cpp/avInfo

# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# Build app
include_directories(${AVTRANSCODER_SRC_PATH})
add_executable(avinfo avInfo.cpp) 
target_link_libraries(avinfo avtranscoder-shared)

# Install app
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/avinfo DESTINATION "bin/")
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/avinfo.man DESTINATION "share/man/man1/")
