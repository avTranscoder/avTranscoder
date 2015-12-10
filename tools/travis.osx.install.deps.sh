#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

brew update

# To build
brew install gcc cmake swig doxygen

# To launch nosetests
brew install python
pip install nose

# To build avplay application
brew install freeglut

# Main dependency
if [[ ${DEPENDENCY_NAME} == "ffmpeg" ]]; then
    brew install ffmpeg
elif [[ ${DEPENDENCY_NAME} == "libav" ]]; then

    wget https://libav.org/releases/libav-${DEPENDENCY_VERSION}.tar.gz
    tar -xvf libav-${DEPENDENCY_VERSION}.tar.gz
    cd libav-${DEPENDENCY_VERSION}
    ./configure --disable-yasm --enable-shared --disable-static && make -k && make install

fi
