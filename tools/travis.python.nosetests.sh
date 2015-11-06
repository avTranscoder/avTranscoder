#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

# Get avtranscoder library
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${DEPENDENCY_INSTALL}/lib
export PYTHONPATH=${AVTRANSCODER_INSTALL}/lib/python2.7/site-packages/:$PYTHONPATH

# Get assets
git clone https://github.com/avTranscoder/avTranscoder-data.git
export AVTRANSCODER_TEST_VIDEO_AVI_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_480p_stereo.avi
export AVTRANSCODER_TEST_VIDEO_MP4_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_HD.mp4
export AVTRANSCODER_TEST_AUDIO_WAVE_FILE=`pwd`/avTranscoder-data/audio/frequenciesPerChannel.wav
export AVTRANSCODER_TEST_AUDIO_MOV_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_1080p_5_1.mov

# Launch tests
nosetests ${TRAVIS_BUILD_DIR}/test/pyTest --with-coverage
