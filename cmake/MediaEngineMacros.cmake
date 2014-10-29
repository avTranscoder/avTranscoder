# Get useful variables
set(MEDIA_ENGINE_APP_PATH "${PROJECT_SOURCE_DIR}/app")
set(MEDIA_ENGINE_SRC_PATH "${PROJECT_SOURCE_DIR}/src")
file(GLOB_RECURSE MEDIA_CORE_SRC_FILES "mediaCore/*.cpp" "mediaCore/*.hpp")
file(GLOB_RECURSE MEDIA_IO_SRC_FILES "mediaIO/*.cpp" "mediaIO/*.hpp")
file(GLOB_RECURSE MEDIA_TRANSFORM_SRC_FILES "mediaTransform/*.cpp" "mediaTransform/*.hpp")


macro(generate_python_binding LIBRARY_NAME)
	if(SWIG_FOUND)
		# Load Modules/UseSWIG.cmake
		include(${SWIG_USE_FILE})

		# Configure Swig
		set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}")
		set(LIBRARY_BINDING_FILE "${LIBRARY_NAME}.i")
		set_source_files_properties(${LIBRARY_BINDING_FILE} PROPERTIES CPLUSPLUS ON)

		### PYTHON BINDING
		find_package(PythonLibs)
		if(PYTHONLIBS_FOUND)
			include_directories(${PYTHON_INCLUDE_PATH})

			if(${CMAKE_VERSION} STRLESS 2.8.8)
				# Get Python version from include path
				string(LENGTH ${PYTHON_INCLUDE_DIR} PYTHON_INCLUDE_DIRS_LENGTH)
				if(WIN32)
					set(PYTHON_INCLUDE_DIRS_LENGTH_EXPRESSION "${PYTHON_INCLUDE_DIRS_LENGTH} - 10")
					set(PYTHON_VERSION_LENGTH_EXPRESSION "2")
				else()
					set(PYTHON_INCLUDE_DIRS_LENGTH_EXPRESSION "${PYTHON_INCLUDE_DIRS_LENGTH} - 3")
					set(PYTHON_VERSION_LENGTH_EXPRESSION "3")
				endif()
				math(EXPR PYTHON_INCLUDE_DIRS_LENGTH ${PYTHON_INCLUDE_DIRS_LENGTH_EXPRESSION} )
				string(SUBSTRING ${PYTHON_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS_LENGTH} ${PYTHON_VERSION_LENGTH_EXPRESSION} PYTHONLIBS_VERSION_STRING)
			endif()

			# Swig flags
			set(CMAKE_SWIG_FLAGS -c++ -fcompact)

			# Create '_library' shared lib (python)
			swig_add_module(${LIBRARY_NAME}-py python ${LIBRARY_BINDING_FILE})
			# For Python binding, need to compile the wrapper into a lib called "_<module_name>.so"
			set_target_properties(${SWIG_MODULE_${LIBRARY_NAME}-py_REAL_NAME} PROPERTIES OUTPUT_NAME _${LIBRARY_NAME})
			if(NOT APPLE AND NOT WIN32)
				set_target_properties(${SWIG_MODULE_${LIBRARY_NAME}-py_REAL_NAME} PROPERTIES SOVERSION ${MEDIA_ENGINE_VERSION_MAJOR})
				set_target_properties(${SWIG_MODULE_${LIBRARY_NAME}-py_REAL_NAME} PROPERTIES VERSION ${MEDIA_ENGINE_VERSION})
			endif()
			swig_link_libraries(${LIBRARY_NAME}-py ${LIBRARY_NAME}-shared ${PYTHON_LIBRARIES})

			# Install python interface
			set(LIBRARY_PYTHON_BINDING_FILE "${CMAKE_SWIG_OUTDIR}/${LIBRARY_NAME}.py")
			set(LIBRARY_PYTHON_MODULE_OUTPUT_DIR "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages/mediaEngine")
			install(
				FILES ${LIBRARY_PYTHON_BINDING_FILE}
				DESTINATION ${LIBRARY_PYTHON_MODULE_OUTPUT_DIR}
			)
			# Install __init__.py
			install(CODE "file(WRITE ${CMAKE_INSTALL_PREFIX}/${LIBRARY_PYTHON_MODULE_OUTPUT_DIR}/__init__.py)")
			# Install library closed to python interface
			install(
				TARGETS ${SWIG_MODULE_${LIBRARY_NAME}-py_REAL_NAME}
				DESTINATION "lib/python${PYTHONLIBS_VERSION_STRING}/site-packages/mediaEngine"
			)
		else()
			message("PYTHON not found, will not build python binding.")
		endif()
	else(SWIG_FOUND)
		message("SWIG not found, will not build python and java bindings.")
	endif(SWIG_FOUND)
endmacro()

macro(generate_java_binding LIBRARY_NAME)
	if(SWIG_FOUND)
		# Load Modules/UseSWIG.cmake
		include(${SWIG_USE_FILE})

		# Configure Swig
		set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}")
		set(LIBRARY_BINDING_FILE "${LIBRARY_NAME}.i")
		set_source_files_properties(${LIBRARY_BINDING_FILE} PROPERTIES CPLUSPLUS ON)

		find_package(Java)
		find_package(JNI)
		if(JAVA_FOUND AND JNI_FOUND)
			include(UseJava)
			include_directories(${JAVA_INCLUDE_PATH})
			include_directories(${JNI_INCLUDE_DIRS})

			# Swig flags
			set(CMAKE_SWIG_FLAGS -c++ -fcompact -package org.${LIBRARY_NAME})

			# Create 'library-java' shared lib
			swig_add_module(${LIBRARY_NAME}-java java ${LIBRARY_BINDING_FILE})
			if(WIN32)
				set_target_properties(${LIBRARY_NAME}-java PROPERTIES OUTPUT_NAME "${LIBRARY_NAME}-java-${MEDIA_ENGINE_VERSION}")
			endif()
			if(NOT APPLE AND NOT WIN32)
				set_target_properties(${SWIG_MODULE_${LIBRARY_NAME}-java_REAL_NAME} PROPERTIES SOVERSION ${MEDIA_ENGINE_VERSION_MAJOR})
				set_target_properties(${SWIG_MODULE_${LIBRARY_NAME}-java_REAL_NAME} PROPERTIES VERSION ${MEDIA_ENGINE_VERSION})
			endif()
			swig_link_libraries(${LIBRARY_NAME}-java ${LIBRARY_NAME}-shared)

			# Post build of java binding : compile java files (generate class files) and create jar
			set(LIBRARY_JAVA_OUTPUT_PATH "${PROJECT_BINARY_DIR}/src/${LIBRARY_NAME}")
			set(LIBRARY_JAVA_CLASS_OUTPUT_PATH "${LIBRARY_JAVA_OUTPUT_PATH}/target")
			set(LIBRARY_JAR_PATH "${PROJECT_BINARY_DIR}/src/${LIBRARY_NAME}")
			set(LIBRARY_JAR_NAME "${LIBRARY_NAME}-${MEDIA_ENGINE_VERSION}.jar")
			execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBRARY_JAVA_CLASS_OUTPUT_PATH})
			
			ADD_CUSTOM_COMMAND(
				TARGET ${LIBRARY_NAME}-java
				POST_BUILD
				COMMAND ${Java_JAVAC_EXECUTABLE} "-d" ${LIBRARY_JAVA_CLASS_OUTPUT_PATH} "\"${LIBRARY_JAVA_OUTPUT_PATH}\"/*.java"
				COMMAND ${Java_JAR_EXECUTABLE} cf ${LIBRARY_JAR_NAME} "-C" ${LIBRARY_JAVA_CLASS_OUTPUT_PATH} "."
			)

			# Install jar file
			install(FILES ${LIBRARY_JAR_PATH}/${LIBRARY_JAR_NAME} DESTINATION "share/java/")
		else()
			message("JAVA not found, will not build java binding.")
		endif()
	else(SWIG_FOUND)
		message("SWIG not found, will not build python and java bindings.")
	endif(SWIG_FOUND)
endmacro()