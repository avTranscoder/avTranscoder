# Load custom cmake utilities
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(AvTranscoderMacros)

# find package ffmpeg/libav
find_package(FFmpeg COMPONENTS avcodec avformat avutil swscale swresample avresample)

# Check if FFmpeg or libav dependency
if(avresample_FOUND)
	add_definitions(-DAVTRANSCODER_LIBAV_DEPENDENCY)
	message("Build avTranscoder with dependency to libav.")
elseif(swresample_FOUND)
	add_definitions(-DAVTRANSCODER_FFMPEG_DEPENDENCY)
        message("Build avTranscoder with dependency to ffmpeg.")
else()
        message(SEND_ERROR "Can't define if you depend on ffmpeg or libav.")
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
target_include_directories(avtranscoder-shared PUBLIC
    ${AVTRANSCODER_SRC_PATH} ${FFMPEG_INCLUDE_DIR})


### Install AvTranscoder libs and include
install(
	DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/AvTranscoder"
	DESTINATION "include"
	FILES_MATCHING
	PATTERN "*.hpp"
	PATTERN "*.i"
)
install(TARGETS avtranscoder-static DESTINATION lib/)
install(TARGETS avtranscoder-shared DESTINATION lib/)

### Install AvTranscoder ressource
install(
	DIRECTORY "${PROJECT_SOURCE_DIR}/ressource"
	DESTINATION "share"
	FILES_MATCHING
	PATTERN "*.prf"
)

### Bindings with SWIG
if(AVTRANSCODER_DISABLE_BINDINGS)
	message("Bindings disabled, will not build bindings.")
	return()
endif()

find_package(SWIG)
if(SWIG_FOUND)
	# Load Modules/UseSWIG.cmake
	include(${SWIG_USE_FILE})

	# Configure Swig
	set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/AvTranscoder")
	set(AVTRANSCODER_BINDING_FILE "AvTranscoder/avTranscoder.i")
	set_source_files_properties(${AVTRANSCODER_BINDING_FILE} PROPERTIES CPLUSPLUS ON)

	### PYTHON BINDING
	if(AVTRANSCODER_DISABLE_PYTHON_BINDING)
		message("PYTHON binding disabled, will not build python binding.")
	else()
		find_package(PythonLibs)
		if(PYTHONLIBS_FOUND)
			include_directories(${PYTHON_INCLUDE_PATH})

			# Swig flags
			set(CMAKE_SWIG_FLAGS -c++ -fcompact)

			# Create '_avtranscoder' shared lib (python)
			swig_add_module(avtranscoder-py python ${AVTRANSCODER_BINDING_FILE})
			# For Python binding, need to compile the wrapper into a lib called "_<module_name>.so"
			set_target_properties(${SWIG_MODULE_avtranscoder-py_REAL_NAME} PROPERTIES OUTPUT_NAME _avtranscoder)
			if(NOT APPLE AND NOT WIN32)
				set_target_properties(${SWIG_MODULE_avtranscoder-py_REAL_NAME} PROPERTIES SOVERSION ${AVTRANSCODER_VERSION_MAJOR})
				set_target_properties(${SWIG_MODULE_avtranscoder-py_REAL_NAME} PROPERTIES VERSION ${AVTRANSCODER_VERSION})
			endif()
			# Force the library to look in the project lib folder relatively
			set_target_properties(${SWIG_MODULE_avtranscoder-py_REAL_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
			if(APPLE)
				set_target_properties(${SWIG_MODULE_avtranscoder-py_REAL_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../../..")
			else()
				set_target_properties(${SWIG_MODULE_avtranscoder-py_REAL_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../../..:$ORIGIN")
			endif()
			swig_link_libraries(avtranscoder-py avtranscoder-shared ${PYTHON_LIBRARIES})

			# Install python interface
			set(AVTRANSCODER_PYTHON_BINDING_FILE "${CMAKE_SWIG_OUTDIR}/avtranscoder.py")
			set(AVTRANSCODER_PYTHON_MODULE_OUTPUT_DIR "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages/pyAvTranscoder/")
			install(
				FILES ${AVTRANSCODER_PYTHON_BINDING_FILE} ${AVTRANSCODER_PYTHON_INIT_FILE}
				DESTINATION ${AVTRANSCODER_PYTHON_MODULE_OUTPUT_DIR}
			)
			# Install python lib and __init__.py files
			install(TARGETS ${SWIG_MODULE_avtranscoder-py_REAL_NAME} DESTINATION "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages/pyAvTranscoder/")
			install(CODE "file(WRITE ${CMAKE_INSTALL_PREFIX}/${AVTRANSCODER_PYTHON_MODULE_OUTPUT_DIR}/__init__.py)")
		else()
			message("PYTHON not found, will not build python binding.")
		endif()
        endif()

	### JAVA BINDING
        if(AVTRANSCODER_DISABLE_JAVA_BINDING)
		message("JAVA binding disabled, will not build java binding.")
        else()
		find_package(Java)
		find_package(JNI)
		if(JAVA_FOUND AND JNI_FOUND)
			include(UseJava)
			include_directories(${JAVA_INCLUDE_PATH})
			include_directories(${JNI_INCLUDE_DIRS})

			# Swig flags
			set(CMAKE_SWIG_FLAGS -c++ -fcompact -package org.avtranscoder)

			# Create 'avtranscoder-java' shared lib
			swig_add_module(avtranscoder-java java ${AVTRANSCODER_BINDING_FILE})
			if(WIN32)
				set_target_properties(avtranscoder-java PROPERTIES OUTPUT_NAME "avtranscoder-java-${AVTRANSCODER_VERSION}")
			endif()
			if(NOT APPLE AND NOT WIN32)
				set_target_properties(${SWIG_MODULE_avtranscoder-java_REAL_NAME} PROPERTIES SOVERSION ${AVTRANSCODER_VERSION_MAJOR})
				set_target_properties(${SWIG_MODULE_avtranscoder-java_REAL_NAME} PROPERTIES VERSION ${AVTRANSCODER_VERSION})
			endif()
			# Force the library to look in the project lib folder relatively
			set_target_properties(${SWIG_MODULE_avtranscoder-java_REAL_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)
			if(APPLE)
				set_target_properties(${SWIG_MODULE_avtranscoder-java_REAL_NAME} PROPERTIES INSTALL_RPATH "@loader_path/..")
			else()
				set_target_properties(${SWIG_MODULE_avtranscoder-java_REAL_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/..:$ORIGIN")
			endif()
			swig_link_libraries(avtranscoder-java avtranscoder-shared)

			# Post build of java binding : compile java files (generate class files) and create jar
			set(AVTRANSCODER_JAVA_OUTPUT_PATH "${PROJECT_BINARY_DIR}/src/AvTranscoder")
			set(AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH "${AVTRANSCODER_JAVA_OUTPUT_PATH}/target")
			set(AVTRANSCODER_JAR_PATH "${PROJECT_BINARY_DIR}/src")
			set(AVTRANSCODER_JAR_NAME "avtranscoder-${AVTRANSCODER_VERSION}.jar")
			execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH})
		
			ADD_CUSTOM_COMMAND(
				TARGET avtranscoder-java
				POST_BUILD
				COMMAND ${Java_JAVAC_EXECUTABLE} "-d" ${AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH} "\"${AVTRANSCODER_JAVA_OUTPUT_PATH}\"/*.java"
				COMMAND ${Java_JAR_EXECUTABLE} cf ${AVTRANSCODER_JAR_NAME} "-C" ${AVTRANSCODER_JAVA_CLASS_OUTPUT_PATH} "."
			)

			# Install java lib and jar files
			install(TARGETS ${SWIG_MODULE_avtranscoder-java_REAL_NAME} DESTINATION "lib/java")
			install(FILES ${AVTRANSCODER_JAR_PATH}/${AVTRANSCODER_JAR_NAME} DESTINATION "share/java/")
		else()
			message("JAVA not found, will not build java binding.")
		endif()
	endif()

else(SWIG_FOUND)
	message("SWIG not found, will not build python and java bindings.")

endif(SWIG_FOUND)

# generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	set(doxyfile_in ${PROJECT_SOURCE_DIR}/doc/code/doxyfile.in)
	set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	
	configure_file(${doxyfile_in} ${doxyfile} @ONLY)
	add_custom_target(doc
		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
endif(DOXYGEN_FOUND)
