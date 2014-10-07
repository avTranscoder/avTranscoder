#!/bin/bash

export PYTHONPATH=`pwd`/build/dist/lib:`pwd`/build/dist/lib/python2.7/site-packages/:$PYTHONPATH

export AVTRANSCODER_TEST_VIDEO_FILE=`pwd`/data/video/video.mxf
export AVTRANSCODER_TEST_AUDIO_FILE=`pwd`/data/audio/audio.wav

cd test/pyTest

nosetests
