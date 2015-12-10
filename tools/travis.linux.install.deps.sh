#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

if [ ! -d "${DEPENDENCY_INSTALL_PATH}/lib/" ]; then

    if [[ ${DEPENDENCY_NAME} == "ffmpeg" ]]; then

        wget https://www.ffmpeg.org/releases/ffmpeg-${DEPENDENCY_VERSION}.tar.bz2
        bunzip2 ffmpeg-${DEPENDENCY_VERSION}.tar.bz2
        tar -xvf ffmpeg-${DEPENDENCY_VERSION}.tar
        cd ffmpeg-${DEPENDENCY_VERSION}
        ./configure --prefix=${DEPENDENCY_INSTALL_PATH} --disable-yasm --enable-shared --disable-static && make && make install

    elif [[ ${DEPENDENCY_NAME} == "libav" ]]; then

        wget https://libav.org/releases/libav-${DEPENDENCY_VERSION}.tar.gz
        tar -xvf libav-${DEPENDENCY_VERSION}.tar.gz
        cd libav-${DEPENDENCY_VERSION}
        ./configure --prefix=${DEPENDENCY_INSTALL_PATH} --disable-yasm --enable-shared --disable-static && make && make install

    fi

fi
