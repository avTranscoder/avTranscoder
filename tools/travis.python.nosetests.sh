#!/bin/bash

# Get avtranscoder library
export PYTHONPATH=`pwd`/build/dist/lib:`pwd`/build/dist/lib/python2.7.6/site-packages/:$PYTHONPATH

# Get avtranscoder profiles
export AVPROFILES=`pwd`/build/dist/share/ressource

# Get assets
git clone https://github.com/avTranscoder/avTranscoder-data.git
export AVTRANSCODER_TEST_VIDEO_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_HD.mp4
export AVTRANSCODER_TEST_AUDIO_FILE=`pwd`/avTranscoder-data/audio/frequenciesPerChannel.wav

# Launch tests
cd test/pyTest
nosetests
