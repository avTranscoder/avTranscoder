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

# Build FFmpeg 2.2.9
sudo wget https://www.ffmpeg.org/releases/ffmpeg-2.2.9.tar.bz2
sudo bunzip2 ffmpeg-2.2.9.tar.bz2
sudo tar -xvf ffmpeg-2.2.9.tar
cd ffmpeg-2.2.9
./configure --disable-yasm --enable-shared --disable-static && make && sudo make install
