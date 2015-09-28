#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

brew update

brew install gcc cmake swig
brew install freeglut doxygen

if [[ ${DEPENDENCY_MODE} == "ffmpeg" ]]; then
    brew install ffmpeg
elif [[ ${DEPENDENCY_MODE} == "libav" ]]; then
    brew install libav
fi
