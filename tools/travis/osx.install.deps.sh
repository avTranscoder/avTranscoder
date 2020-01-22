#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

brew update

# To build
brew install gcc cmake swig doxygen

# To launch nosetests
brew install pyenv
pyenv install 3.5.0
pyenv local 3.5.0
pip3 install nose

# To build avplay application
brew install freeglut

# Main dependency

ls -l ${DEPENDENCY_INSTALL_PATH}/lib/
./tools/travis/linux.install.deps.sh
