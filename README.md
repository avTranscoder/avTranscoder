# avTranscoder

C++ API for Libav / FFmpeg

Based on Libav/FFmpeg libraries to support various video and audio formats, avTranscoder provides the high level API to re-wrap or transcode media easily.

#### What you need to know
* C++ library
* Java and Python bindings generated with SWIG
* multiplateform (Linux, MAC, Windows)
* your call to be based on Libav, FFmpeg, or your custom fork of one of these librairies

#### License
See [**COPYING.md**](COPYING.md)

#### How to use
Check out applications contained in the project to see examples of how to use the library in C++, Java or Python.

To encode, avTranscoder manipulates profiles.
A profile is a text file which discribes, with a set of key-value, what we want as output for the format, the video, or the audio.
You can create your own profiles and export a variable called ```AVPROFILES``` to indicate the path to them.

The minimum format profile is:
```
avProfileName=profileName
avProfileLongName=profileLongName
avProfileType=avProfileTypeFormat
format=formatName 
```

The minimum video profile is:
```
avProfileName=profileName
avProfileLongName=profileLongName
avProfileType=avProfileTypeVideo
codec=codecName
```

The minimum audio profile is:
```
avProfileName=profileName
avProfileLongName=profileLongName
avProfileType=avProfileTypeAudio
codec=codecName
```

#### Continuous Integration

###### Drone.io  
[![Build Status](https://drone.io/github.com/avTranscoder/avTranscoder/status.png)](https://drone.io/github.com/avTranscoder/avTranscoder/latest)

###### Travis
[![Build Status](https://travis-ci.org/avTranscoder/avTranscoder.svg?branch=master)](https://travis-ci.org/avTranscoder/avTranscoder)

###### Coverity Scan  
<a href="https://scan.coverity.com/projects/2626">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/2626/badge.svg"/>
</a>

#### Tests

###### nosetests
Python tests using nosetests.

Create environment variables to use your files in tests.
* ```AVTRANSCODER_TEST_AUDIO_FILE```
* ```AVTRANSCODER_TEST_VIDEO_FILE```

Note: for continuous integration, we launch tests with media files contained in ```avTranscoder-data``` repository.

Launch the tests:
```
cd test/pyTest
nosetests
```

#### Packaging

###### Build openSUSE  
comming soon

