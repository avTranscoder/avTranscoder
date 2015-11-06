#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

lsb_release -a

if [ ! -d "${DEPENDENCY_INSTALL}/lib/" ]; then

    if [[ ${DEPENDENCY_MODE} == "ffmpeg" ]]; then

        export FFMPEG_VERSION=2.2.9
        wget https://www.ffmpeg.org/releases/ffmpeg-${FFMPEG_VERSION}.tar.bz2
        bunzip2 ffmpeg-${FFMPEG_VERSION}.tar.bz2
        tar -xvf ffmpeg-${FFMPEG_VERSION}.tar
        cd ffmpeg-${FFMPEG_VERSION}
        ./configure --prefix=${DEPENDENCY_INSTALL} --disable-yasm --enable-shared --disable-static && make && make install

    elif [[ ${DEPENDENCY_MODE} == "libav" ]]; then

        export LIBAV_VERSION=11.3
        wget https://libav.org/releases/libav-${LIBAV_VERSION}.tar.gz
        tar -xvf libav-${LIBAV_VERSION}.tar.gz
        cd libav-${LIBAV_VERSION}
        ./configure --prefix=${DEPENDENCY_INSTALL} --disable-yasm --enable-shared --disable-static && make && make install

    fi

fi
