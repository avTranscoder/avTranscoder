#!/bin/bash

# Get mediaManager library
export PYTHONPATH=`pwd`/build/dist/lib/python2.7/site-packages/:$PYTHONPATH
export LD_LIBRARY_PATH=`pwd`/build/dist/lib:$LD_LIBRARY_PATH

# FFmpeg libraries
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Get assets
git clone https://github.com/avTranscoder/avTranscoder-data.git
export MEDIA_MANAGER_TEST_VIDEO_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_480p_stereo.avi
export MEDIA_MANAGER_TEST_AUDIO_FILE=`pwd`/avTranscoder-data/audio/frequenciesPerChannel.wav

# Launch tests
cd test/pyTest
nosetests
