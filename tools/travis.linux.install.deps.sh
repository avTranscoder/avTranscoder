#!/bin/bash
lsb_release -a

sudo apt-add-repository "deb http://archive.ubuntu.com/ubuntu trusty main restricted universe multiverse"
sudo apt-add-repository "deb http://archive.ubuntu.com/ubuntu trusty-updates main restricted universe multiverse"
sudo apt-add-repository "deb http://security.ubuntu.com/ubuntu trusty-security main restricted universe multiverse"
sudo add-apt-repository -y ppa:motumedia/libav-daily
sudo apt-get update
sudo apt-get install -qq gcc g++ cmake swig swig2.0
sudo apt-get install -qq python-nose
sudo apt-get install -qq libavcodec56 libavformat56 libavutil54 libswscale3 libavresample2 python-dev freeglut3-dev libxmu-dev doxygen
