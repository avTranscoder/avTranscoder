#!/bin/bash
lsb_release -a

sudo apt-add-repository "deb http://archive.ubuntu.com/ubuntu trusty main restricted universe multiverse"
sudo apt-add-repository "deb http://archive.ubuntu.com/ubuntu trusty-updates main restricted universe multiverse"
sudo apt-add-repository "deb http://security.ubuntu.com/ubuntu trusty-security main restricted universe multiverse"
sudo apt-get update -qq
sudo apt-get install -qq gcc g++ cmake swig swig2.0
sudo apt-get install -qq python-dev doxygen
sudo apt-get install -qq freeglut3-dev libxmu-dev
sudo apt-get install -qq python-nose

if [[ ${DEPENDENCY_MODE} == "ffmpeg" ]]; then
    export FFMPEG_VERSION=2.2.9
    sudo wget https://www.ffmpeg.org/releases/ffmpeg-${FFMPEG_VERSION}.tar.bz2
    sudo bunzip2 ffmpeg-${FFMPEG_VERSION}.tar.bz2
    sudo tar -xvf ffmpeg-${FFMPEG_VERSION}.tar
    cd ffmpeg-${FFMPEG_VERSION}
    sudo ./configure --prefix=${DEPENDENCY_INSTALL} --disable-yasm --enable-shared --disable-static && sudo make && sudo make install

elif [[ ${DEPENDENCY_MODE} == "libav" ]]; then
    export LIBAV_VERSION=11.3
    sudo wget https://libav.org/releases/libav-${LIBAV_VERSION}.tar.gz
    sudo tar -xvf libav-${LIBAV_VERSION}.tar.gz
    cd libav-${LIBAV_VERSION}
    sudo ./configure --prefix=${DEPENDENCY_INSTALL} --disable-yasm --enable-shared --disable-static && sudo make && sudo make install

fi
