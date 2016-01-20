#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

# Create directory of build
mkdir -p ${AVTRANSCODER_BUILD_PATH}
cd ${AVTRANSCODER_BUILD_PATH}

# Ask cmake to search in all dependencies we've installed manually
export CMAKE_PREFIX_PATH=${DEPENDENCY_INSTALL_PATH}

# Build avTranscoder
cmake .. -DCMAKE_INSTALL_PREFIX=${AVTRANSCODER_INSTALL_PATH} -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH -DCMAKE_BUILD_TYPE=Release -DAVTRANSCODER_PYTHON_VERSION_OF_BINDING=2.7 -DAVTRANSCODER_COVERAGE=${ENABLE_COVERAGE}
make -k
make install
