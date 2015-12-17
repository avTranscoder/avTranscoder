#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

brew update

# To build
brew install gcc cmake swig doxygen

# To launch nosetests
brew install python
pip install nose

# To build avplay application
brew install freeglut

# Main dependency
./tools/travis.linux.install.deps.sh
