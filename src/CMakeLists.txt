# Load project cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Get useful variables
set(AVTRANSCODER_APP_PATH "${PROJECT_SOURCE_DIR}/app" PARENT_SCOPE)
set(AVTRANSCODER_SRC_PATH "${PROJECT_SOURCE_DIR}/src" PARENT_SCOPE)
file(GLOB_RECURSE AVTRANSCODER_SRC_FILES "AvTranscoder/*.cpp" "AvTranscoder/*.hpp")

# Get FFMPEG from parameters
set(FFMPEG_INCLUDE_DIR "/path/to/ffmpeg/include/" CACHE STRING "")
set(FFMPEG_LIBRARY_DIR "/path/to/ffmpeg/lib/" CACHE STRING "")
file(GLOB_RECURSE FFMPEG_LIBS "${FFMPEG_LIBRARY_DIR}/*.so")

# Include AvTranscoder and FFmpeg
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${FFMPEG_INCLUDE_DIR})

# Create 'avtranscoder' static lib
add_library(avtranscoder-static STATIC ${AVTRANSCODER_SRC_FILES})
set_target_properties(avtranscoder-static PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(avtranscoder-static PROPERTIES OUTPUT_NAME avtranscoder)
target_link_libraries(avtranscoder-static ${FFMPEG_LIBS})

# Create 'avtranscoder' shared lib
add_library(avtranscoder-shared SHARED ${AVTRANSCODER_SRC_FILES})
set_target_properties(avtranscoder-shared PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(avtranscoder-shared PROPERTIES OUTPUT_NAME avtranscoder)
set_target_properties(avtranscoder-shared PROPERTIES SOVERSION ${AVTRANSCODER_VERSION})
target_link_libraries(avtranscoder-shared ${FFMPEG_LIBS})

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
set(AVTRANSCODER_LIBRARY_OUTPUT_PATH "${LIBRARY_OUTPUT_PATH}")
file(GLOB_RECURSE AVTRANSCODER_OUTPUT_LIBRARIES "${AVTRANSCODER_LIBRARY_OUTPUT_PATH}/*")
install(FILES ${AVTRANSCODER_OUTPUT_LIBRARIES} DESTINATION "lib/")

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
	set(ENV{JAVA_HOME} "/usr/java/jdk1.6.0_43") # fix find java SDK
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

	# Need to generate java files before class files and jar
	set(AVTRANSCODER_JAVA_OUTPUT_PATH "${PROJECT_BINARY_DIR}/src/AvTranscoder")
	if(EXISTS "${AVTRANSCODER_JAVA_OUTPUT_PATH}/Transcoder.java")
		file(GLOB AVTRANSCODER_JAVA_BINDING_FILES "${AVTRANSCODER_JAVA_OUTPUT_PATH}/*.java")
		add_jar(avtranscoder-jar ${AVTRANSCODER_JAVA_BINDING_FILES})

		# Install jar
		install_jar(avtranscoder-jar "share/java/")
	endif()

else(SWIG_FOUND)
	message("SWIG not found, will not build python and java bindings.")

endif(SWIG_FOUND)
