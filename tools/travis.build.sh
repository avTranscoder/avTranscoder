#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

# Create directory of build
mkdir -p ${AVTRANSCODER_BUILD_PATH}
cd ${AVTRANSCODER_BUILD_PATH}

# Customize environment
if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    # Ask cmake to search in all dependencies we've installed manually
    export CMAKE_PREFIX_PATH=${DEPENDENCY_INSTALL_PATH}
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    # Ask cmake to search in all homebrew packages
    export CMAKE_PREFIX_PATH=$(echo /usr/local/Cellar/*/* | sed 's/ /;/g')
fi

# Build avTranscoder
cmake .. -DCMAKE_INSTALL_PREFIX=${AVTRANSCODER_INSTALL_PATH} -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH -DCMAKE_BUILD_TYPE=Release -DAVTRANSCODER_PYTHON_VERSION_OF_BINDING=2.7 -DAVTRANSCODER_COVERAGE=${ENABLE_COVERAGE}
make -j${CI_NODE_TOTAL}
make install
