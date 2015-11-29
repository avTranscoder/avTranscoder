#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

brew update

# To build
brew install gcc cmake swig doxygen

# To generate python binding
if [[ ${PYTHON_VERSION} == "2.7" ]]; then
    brew install python
elif [[ ${PYTHON_VERSION} == "2.6" ]]; then
    brew install python26
fi

# To launch nosetests
pip install nose

# To build avplay application
brew install freeglut

# Main dependency
if [[ ${DEPENDENCY_MODE} == "ffmpeg" ]]; then
    brew install ffmpeg
elif [[ ${DEPENDENCY_MODE} == "libav" ]]; then
    brew install libav
fi
