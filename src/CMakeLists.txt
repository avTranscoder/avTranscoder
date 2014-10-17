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
if(WIN32)
	set_target_properties(avtranscoder-shared PROPERTIES OUTPUT_NAME "avtranscoder-${AVTRANSCODER_VERSION}")
else()
	set_target_properties(avtranscoder-shared PROPERTIES OUTPUT_NAME avtranscoder)
endif()
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
	if(PYTHONLIBS_FOUND)
		include_directories(${PYTHON_INCLUDE_PATH})

		# Get Python version from include path
		string(LENGTH ${PYTHON_INCLUDE_DIR} PYTHON_INCLUDE_DIRS_LENGTH)
		math(EXPR PYTHON_INCLUDE_DIRS_LENGTH "${PYTHON_INCLUDE_DIRS_LENGTH} - 3" )
		string(SUBSTRING ${PYTHON_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS_LENGTH} 3 PYTHONLIBS_VERSION_STRING)

		# Swig flags
		set(CMAKE_SWIG_FLAGS -c++ -fcompact)

		# Create 'avtranscoder-py' shared lib
		swig_add_module(avtranscoder-py python ${AVTRANSCODER_BINDING_FILE})
		if(NOT APPLE AND NOT WIN32)
			set_target_properties(${SWIG_MODULE_avtranscoder-py_REAL_NAME} PROPERTIES SOVERSION ${AVTRANSCODER_VERSION_MAJOR})
			set_target_properties(${SWIG_MODULE_avtranscoder-py_REAL_NAME} PROPERTIES VERSION ${AVTRANSCODER_VERSION})
		endif()
		swig_link_libraries(avtranscoder-py avtranscoder-shared ${PYTHON_LIBRARIES})

		# Install python interface
		set(AVTRANSCODER_PYTHON_BINDING_FILE "${CMAKE_SWIG_OUTDIR}/AvTranscoder.py")
		install(
			FILES ${AVTRANSCODER_PYTHON_BINDING_FILE}
			RENAME "avtranscoder.py"
			DESTINATION "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages/avtranscoder/"
		)
	else()
		message("PYTHON not found, will not build python binding.")
	endif()

	### JAVA BINDING
	find_package(Java)
	find_package(JNI)
	if(JAVA_FOUND AND JNI_FOUND)
		include(UseJava)
		include_directories(${JAVA_INCLUDE_PATH})
		include_directories(${JNI_INCLUDE_DIRS})

		# Swig flags
		set(CMAKE_SWIG_FLAGS -c++ -fcompact -package org.AvTranscoder)

		# Create 'avtranscoder-java' shared lib
		swig_add_module(avtranscoder-java java ${AVTRANSCODER_BINDING_FILE})
		if(NOT APPLE AND NOT WIN32)
			set_target_properties(${SWIG_MODULE_avtranscoder-java_REAL_NAME} PROPERTIES SOVERSION ${AVTRANSCODER_VERSION_MAJOR})
			set_target_properties(${SWIG_MODULE_avtranscoder-java_REAL_NAME} PROPERTIES VERSION ${AVTRANSCODER_VERSION})
		endif()
		swig_link_libraries(avtranscoder-java avtranscoder-shared)

		# Post build of java binding : compile java files (generate class files) and create jar
		set(AVTRANSCODER_JAVA_OUTPUT_PATH "${PROJECT_BINARY_DIR}/src/AvTranscoder")
		set(AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH "${AVTRANSCODER_JAVA_OUTPUT_PATH}/target")
		set(AVTRANSCODER_JAR_PATH "${PROJECT_BINARY_DIR}/src")
		set(AVTRANSCODER_JAR_NAME "avtranscoder.jar")
		execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH})
		
		ADD_CUSTOM_COMMAND(
			TARGET avtranscoder-java
			POST_BUILD
			COMMAND ${Java_JAVAC_EXECUTABLE} "-d" ${AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH} "\"${AVTRANSCODER_JAVA_OUTPUT_PATH}\"/*.java"
			COMMAND ${Java_JAR_EXECUTABLE} cf ${AVTRANSCODER_JAR_NAME} "-C" ${AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH} "."
		)

		# Install jar file
		install(FILES ${AVTRANSCODER_JAR_PATH}/${AVTRANSCODER_JAR_NAME} DESTINATION "share/java/")
	else()
		message("JAVA not found, will not build java binding.")
	endif()

else(SWIG_FOUND)
	message("SWIG not found, will not build python and java bindings.")

endif(SWIG_FOUND)
