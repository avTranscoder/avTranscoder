@echo on

set PWD=C:\projects\avtranscoder

:: Get avtranscoder library
set PYTHONPATH=%AVTRANSCODER_INSTALL_PATH%\lib\python3.5\site-packages;%PYTHONPATH%
set PATH=%DEPENDENCY_INSTALL_PATH%\bin;%AVTRANSCODER_INSTALL_PATH%\lib;%PATH%

:: Get avtranscoder profiles
set AVPROFILES=%AVTRANSCODER_INSTALL_PATH%\share\avprofiles

:: Get assets
git clone https://github.com/avTranscoder/avTranscoder-data.git
set AVTRANSCODER_TEST_VIDEO_AVI_FILE=%PWD%\avTranscoder-data/video\BigBuckBunny\BigBuckBunny_480p_stereo.avi
set AVTRANSCODER_TEST_VIDEO_MP4_FILE=%PWD%\avTranscoder-data\video\BigBuckBunny\BigBuckBunny_HD.mp4
set AVTRANSCODER_TEST_VIDEO_MOV_FILE=%PWD%\avTranscoder-data\video\BigBuckBunny\BigBuckBunny_640p_h264.mov
set AVTRANSCODER_TEST_AUDIO_WAVE_FILE=%PWD%\avTranscoder-data\audio\frequenciesPerChannel.wav
set AVTRANSCODER_TEST_AUDIO_WAVE_MONO_FILE=%PWD%\avTranscoder-data\audio\frequenciesOneChannel.wav
set AVTRANSCODER_TEST_AUDIO_MOV_FILE=%PWD%\avTranscoder-data\video\BigBuckBunny\BigBuckBunny_1080p_5_1.mov
set AVTRANSCODER_TEST_IMAGE_PNG_FILE=%PWD%\avTranscoder-data\image\BigBuckBunny\bbb-splash.thumbnail.png
set AVTRANSCODER_TEST_IMAGE_JPG_FILE=%PWD%\avTranscoder-data\image\BigBuckBunny\title_anouncement.thumbnail.jpg

:: Launch tests
cd test\pyTest
python3 -m nose
cd ..\..

@echo off
