#!/bin/bash

# Get avtranscoder library
export PYTHONPATH=`pwd`/build/dist/lib/python2.7.6/site-packages/:$PYTHONPATH

# Get avtranscoder profiles
export AVPROFILES=`pwd`/build/dist/share/ressource

# Get assets
git clone https://github.com/avTranscoder/avTranscoder-data.git
export AVTRANSCODER_TEST_VIDEO_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_480p_stereo.avi
export AVTRANSCODER_TEST_AUDIO_WAVE_FILE=`pwd`/avTranscoder-data/audio/frequenciesPerChannel.wav
export AVTRANSCODER_TEST_AUDIO_MOV_FILE=`pwd`/avTranscoder-data/video/BigBuckBunny/BigBuckBunny_1080p_5_1.mov

# Launch tests
cd test/pyTest
nosetests
