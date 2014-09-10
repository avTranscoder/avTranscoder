# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

##find_package(FFmpeg)
find_package(FFmpeg
	COMPONENTS avcodec avformat avutil swscale swresample avresample)

# Check if FFmpeg or libav dependency
if(EXISTS "${FFMPEG_INCLUDE_DIR}/libavresample/avresample.h")
	add_definitions(-DAV_RESAMPLE_LIBRARY)
else()
	add_definitions(-DFF_RESAMPLE_LIBRARY)
endif()

# Include AvTranscoder and FFmpeg
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${FFMPEG_INCLUDE_DIR})

# Create 'avtranscoder' static lib
add_library(avtranscoder-static STATIC ${AVTRANSCODER_SRC_FILES})
set_target_properties(avtranscoder-static PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(avtranscoder-static PROPERTIES OUTPUT_NAME avtranscoder)
target_link_libraries(avtranscoder-static ${FFMPEG_LIBRARIES})

# Create 'avtranscoder' shared lib
add_library(avtranscoder-shared SHARED ${AVTRANSCODER_SRC_FILES})
set_target_properties(avtranscoder-shared PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(avtranscoder-shared PROPERTIES OUTPUT_NAME avtranscoder)
set_target_properties(avtranscoder-shared PROPERTIES SOVERSION ${AVTRANSCODER_VERSION_MAJOR})
set_target_properties(avtranscoder-shared PROPERTIES VERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avtranscoder-shared ${FFMPEG_LIBRARIES})

### Install AvTranscoder libs and include
install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/AvTranscoder"
	DESTINATION "include/"
	FILES_MATCHING
	PATTERN "*.hpp"
	PATTERN "*.i"
	PATTERN "Profiles" EXCLUDE
)
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
install(
	DIRECTORY "${LIBRARY_OUTPUT_PATH}"
	DESTINATION "${CMAKE_INSTALL_PREFIX}"
)

### SWIG
find_package(SWIG)
if(SWIG_FOUND)
	# Load Modules/UseSWIG.cmake
	include(${SWIG_USE_FILE})

	# Configure Swig
	set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/AvTranscoder")
	set(AVTRANSCODER_BINDING_FILE "AvTranscoder/avTranscoder.i")
	set_source_files_properties(${AVTRANSCODER_BINDING_FILE} PROPERTIES CPLUSPLUS ON)

	### PYTHON BINDING
	find_package(PythonLibs)
	include_directories(${PYTHON_INCLUDE_PATH})

	# Swig flags
	set(CMAKE_SWIG_FLAGS -c++ -fcompact)

	# Create 'avtranscoder-py' shared lib
	swig_add_module(avtranscoder-py python ${AVTRANSCODER_BINDING_FILE})
	swig_link_libraries(avtranscoder-py avtranscoder-shared ${PYTHON_LIBRARIES})

	# Install python interface
	file(GLOB AVTRANSCODER_PYTHON_BINDING_FILE "${CMAKE_SWIG_OUTDIR}/*.py")
	install(FILES ${AVTRANSCODER_PYTHON_BINDING_FILE} DESTINATION "lib/python2.7/site-packages/AvTranscoder/")

	### JAVA BINDING
	find_package(Java REQUIRED)
	find_package(JNI REQUIRED)
	include(UseJava)
	include_directories(${JAVA_INCLUDE_PATH})
	include_directories(${JNI_INCLUDE_DIRS})

	# Swig flags
	set(CMAKE_SWIG_FLAGS -c++ -fcompact -package org.AvTranscoder)

	# Create 'avtranscoder-java' shared lib
	swig_add_module(avtranscoder-java java ${AVTRANSCODER_BINDING_FILE})
	swig_link_libraries(avtranscoder-java avtranscoder-shared)

	# Post build of java binding : compile java files (generate class files) and create jar
	set(AVTRANSCODER_JAVA_OUTPUT_PATH "${PROJECT_BINARY_DIR}/src/AvTranscoder")
	set(AVTRANSCODER_JAR_PATH "${PROJECT_BINARY_DIR}/src")
	set(AVTRANSCODER_JAR_NAME "avtranscoder.jar")
	ADD_CUSTOM_COMMAND(
		TARGET avtranscoder-java
		POST_BUILD
		COMMAND ${Java_JAVAC_EXECUTABLE} ${AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH} "${AVTRANSCODER_JAVA_OUTPUT_PATH}/*.java"
		COMMAND ${Java_JAR_EXECUTABLE} cvf ${AVTRANSCODER_JAR_NAME} "${AVTRANSCODER_JAVA_OUTPUT_PATH}/*.class"
	)

	# Install jar file
	install(FILES ${AVTRANSCODER_JAR_PATH}/${AVTRANSCODER_JAR_NAME} DESTINATION "share/java/")

else(SWIG_FOUND)
	message("SWIG not found, will not build python and java bindings.")

endif(SWIG_FOUND)
