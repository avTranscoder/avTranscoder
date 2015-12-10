# avTranscoder

C++ API for Libav / FFmpeg

Based on Libav/FFmpeg libraries to support various video and audio formats, avTranscoder provides the high level API to re-wrap or transcode media easily.

[![Build Status](https://travis-ci.org/avTranscoder/avTranscoder.svg?branch=develop)](https://travis-ci.org/avTranscoder/avTranscoder)
[![Coverage Status](https://coveralls.io/repos/avTranscoder/avTranscoder/badge.svg)](https://coveralls.io/r/avTranscoder/avTranscoder)
<a href="https://scan.coverity.com/projects/2626"><img alt="Coverity Scan Build Status" src="https://scan.coverity.com/projects/2626/badge.svg"/></a>

#### What you need to know
* C++ library
* Java and Python bindings generated with SWIG
* multiplateform (Linux, MAC, Windows)
* your call to be based on Libav, FFmpeg, or your custom fork of one of these librairies

[![Stories in Progress](https://badge.waffle.io/avTranscoder/avTranscoder.svg?label=2 - Working&title=In Progress)](http://waffle.io/avTranscoder/avTranscoder)   
Click on the badge above to have a big picture view of what's in progress and how you can help.  

#### License
See [**COPYING.md**](COPYING.md)

#### How to use
See [**USAGE.md**](USAGE.md)

#### Documentation
See [**Doxygen documentation**](http://avtranscoder.github.io/avTranscoder-doxygen/)

#### Compilation
See [**INSTALL.md**](INSTALL.md)

#### Tests

###### nosetests
Python tests using nosetests.
```
cd test/pyTest
nosetests
```

Some tests need environment variables to be executed (otherwise they will be skipped):
* ```AVTRANSCODER_TEST_VIDEO_FILE```
* ```AVTRANSCODER_TEST_AUDIO_WAVE_FILE```
* ```AVTRANSCODER_TEST_AUDIO_MOV_FILE```

Note: for continuous integration, we launch tests with media files contained in ```avTranscoder-data``` repository.


#### Packaging

###### Build openSUSE  
comming soon



[![Bitdeli Badge](https://d2weczhvl823v0.cloudfront.net/avTranscoder/avtranscoder/trend.png)](https://bitdeli.com/free "Bitdeli Badge")

