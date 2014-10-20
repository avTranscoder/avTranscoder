#!/bin/bash
lsb_release -a

sudo apt-add-repository "deb http://archive.ubuntu.com/ubuntu trusty main restricted universe multiverse"
sudo apt-add-repository "deb http://archive.ubuntu.com/ubuntu trusty-updates main restricted universe multiverse"
sudo apt-add-repository "deb http://security.ubuntu.com/ubuntu trusty-security main restricted universe multiverse"
sudo apt-get update -qq
sudo apt-get install -qq gcc g++ cmake swig swig2.0
sudo apt-get install -qq libavdevice-dev libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libavresample-dev python-dev freeglut3-dev libxmu-dev doxygen
