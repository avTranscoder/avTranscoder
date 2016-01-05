@echo on

set PWD=C:\projects\avtranscoder

:: Get avtranscoder library
set PYTHONPATH=%PWD%\build\lib\python2.7\site-packages;%PYTHONPATH%
set PATH=C:\ProgramData\ffmpeg-%FFMPEG_VERSION%\bin;%PWD%\build\lib;%PATH%

:: Get avtranscoder profiles
set AVPROFILES=%PWD%\build\share\avprofiles

:: Get assets
git clone https://github.com/avTranscoder/avTranscoder-data.git
set AVTRANSCODER_TEST_VIDEO_AVI_FILE=%PWD%\avTranscoder-data/video\BigBuckBunny\BigBuckBunny_480p_stereo.avi
set AVTRANSCODER_TEST_VIDEO_MP4_FILE=%PWD%\avTranscoder-data\video\BigBuckBunny\BigBuckBunny_HD.mp4
set AVTRANSCODER_TEST_VIDEO_MOV_FILE=%PWD%\avTranscoder-data\video\BigBuckBunny\BigBuckBunny_640p_h264.mov
set AVTRANSCODER_TEST_AUDIO_WAVE_FILE=%PWD%\avTranscoder-data\audio\frequenciesPerChannel.wav
set AVTRANSCODER_TEST_AUDIO_MOV_FILE=%PWD%\avTranscoder-data\video\BigBuckBunny\BigBuckBunny_1080p_5_1.mov

:: Launch tests
cd test\pyTest
nosetests

@echo off
