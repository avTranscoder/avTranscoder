#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

brew update

# To build
brew install gcc cmake swig doxygen

# To launch nosetests
export CFLAGS="-I$(brew --prefix openssl)/include"
export LDFLAGS="-L$(brew --prefix openssl)/lib"
brew install pyenv
pyenv install -v 3.5.0
pyenv local 3.5.0
pip3 install nose

# To build avplay application
brew install freeglut

# Main dependency

rm -Rf ${DEPENDENCY_INSTALL_PATH}/*
./tools/travis/linux.install.deps.sh
