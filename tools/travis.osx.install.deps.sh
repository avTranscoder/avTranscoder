#!/bin/bash

brew update

brew install gcc cmake swig
brew install freeglut doxygen

if [[ ${DEPENDENCY_MODE} == "ffmpeg" ]]; then
    brew install ffmpeg

elif [[ ${DEPENDENCY_MODE} == "libav" ]]; then
    brew install libav
