#!/bin/bash

# Get avtranscoder library
export PYTHONPATH=`pwd`/build/dist/lib:`pwd`/build/dist/lib/python2.7/site-packages/:$PYTHONPATH

# Get assets
git clone https://github.com/avTranscoder/avTranscoder-data.git
export AVTRANSCODER_TEST_VIDEO_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_480p_stereo.avi
export AVTRANSCODER_TEST_AUDIO_FILE=`pwd`/avTranscoder-data/audio/frequenciesPerChannel.wav

# Launch tests
cd test/pyTest
nosetests
