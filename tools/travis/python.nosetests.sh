#!/bin/bash

# Print commands and their arguments as they are executed.
set -x

# Get avtranscoder library
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${DEPENDENCY_INSTALL_PATH}/lib
export PYTHONPATH=${AVTRANSCODER_INSTALL_PATH}/lib/python3.5/site-packages/:$PYTHONPATH

# Get assets
git clone https://github.com/avTranscoder/avTranscoder-data.git
export AVTRANSCODER_TEST_VIDEO_AVI_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_480p_stereo.avi
export AVTRANSCODER_TEST_VIDEO_MP4_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_HD.mp4
export AVTRANSCODER_TEST_VIDEO_MOV_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_640p_h264.mov
export AVTRANSCODER_TEST_VIDEO_RAW_FILE=`pwd`/avTranscoder-data/video/Intro/Intro_raw_1080p.h264
export AVTRANSCODER_TEST_AUDIO_WAVE_FILE=`pwd`/avTranscoder-data/audio/frequenciesPerChannel.wav
export AVTRANSCODER_TEST_AUDIO_WAVE_MONO_FILE=`pwd`/avTranscoder-data/audio/frequenciesOneChannel.wav
export AVTRANSCODER_TEST_AUDIO_MOV_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_1080p_5_1.mov
export AVTRANSCODER_TEST_IMAGE_PNG_FILE=`pwd`/avTranscoder-data/image/BigBuckBunny/bbb-splash.thumbnail.png
export AVTRANSCODER_TEST_IMAGE_JPG_FILE=`pwd`/avTranscoder-data/image/BigBuckBunny/title_anouncement.thumbnail.jpg

# Launch tests
nosetests3 -w ${TRAVIS_BUILD_DIR}/test/pyTest --with-coverage --nocapture --verbose > progress.txt
