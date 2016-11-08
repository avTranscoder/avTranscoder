FROM gliderlabs/alpine:3.3

ADD . /src

RUN apk add --update cmake make g++ ffmpeg swig ffmpeg-dev doxygen python-dev freeglut freeglut-dev
RUN mkdir /src/build

WORKDIR /src/build

RUN cmake ..
RUN make
RUN make install


# -- Build avTranscoder with dependency to ffmpeg.
# -- Found SWIG: /usr/bin/swig (found version "3.0.7") 
# -- Could NOT find PythonLibs (missing:  PYTHON_LIBRARIES PYTHON_INCLUDE_DIRS) 
# PYTHON not found, will not build python binding.
# -- Could NOT find Java (missing:  Java_JAVA_EXECUTABLE Java_JAR_EXECUTABLE Java_JAVAC_EXECUTABLE Java_JAVAH_EXECUTABLE Java_JAVADOC_EXECUTABLE) 
# -- Could NOT find JNI (missing:  JAVA_AWT_LIBRARY JAVA_JVM_LIBRARY JAVA_INCLUDE_PATH JAVA_INCLUDE_PATH2 JAVA_AWT_INCLUDE_PATH) 
# JAVA not found, will not build java binding.
# -- Could NOT find Doxygen (missing:  DOXYGEN_EXECUTABLE) 
# -- Could NOT find OpenGL (missing:  OPENGL_gl_LIBRARY OPENGL_INCLUDE_DIR) 
# OpenGL not found, will not build avplayer app.