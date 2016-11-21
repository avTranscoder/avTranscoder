# Compilation

AvTranscoder uses [CMake](http://www.cmake.org/) as build system.

#### Dependencies
AvTranscoder can depend on ffmpeg, libav, or any fork of these projects that follow the same API.
* Tested ffmpeg versions: 2.4.2, 2.4.5, 2.5.7, 2.6.8, 2.7.6
* Recommended libav versions: none

#### To build
```
cmake .
make
make install
```

#### To indicate a specific ffmpeg/libav folder
```
cmake . -DCMAKE_PREFIX_PATH=/path/to/your/install
```

#### To not build apps
```
cmake . -DAVTRANSCODER_DISABLE_APPS=True
```

#### To not build bindings
```
cmake . -DAVTRANSCODER_DISABLE_BINDINGS=True
```
```
cmake . -DAVTRANSCODER_DISABLE_PYTHON_BINDING=True
```
```
cmake . -DAVTRANSCODER_DISABLE_JAVA_BINDING=True
```

#### Other useful flags
###### To install at a specific location
```
cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install
```
###### To build Release/Debug version
```
cmake .. -DCMAKE_BUILD_TYPE=Release/Debug
```
###### To build python binding with a specific python version
```
cmake .. -DAVTRANSCODER_PYTHON_VERSION_OF_BINDING=2.7
```

#### To install on Mac OSX

###### Install homebrew
http://brew.sh/

###### Install avTranscoder
Last stable version:
```
brew tap avTranscoder/official
brew install avtranscoder
```

From develop branch:
```
brew tap avTranscoder/official
brew install --devel avtranscoder
```

###### Use homebrew to install only dependencies
```
brew deps avtranscoder
brew install avtranscoder --only-dependencies
```

#### Tests

###### nosetests
Python tests using nosetests.
```
cd test/pyTest
nosetests
```

Some tests need environment variables to be executed (otherwise they will be skipped):
* ```AVTRANSCODER_TEST_VIDEO_AVI_FILE```
* ```AVTRANSCODER_TEST_VIDEO_MP4_FILE```
* ```AVTRANSCODER_TEST_VIDEO_MOV_FILE```
* ```AVTRANSCODER_TEST_AUDIO_WAVE_FILE```
* ```AVTRANSCODER_TEST_AUDIO_MOV_FILE```
* ```AVTRANSCODER_TEST_IMAGE_PNG_FILE```
* ```AVTRANSCODER_TEST_IMAGE_JPG_FILE```

Note: for continuous integration, we launch tests with media files contained in [avTranscoder-data](https://github.com/avTranscoder/avTranscoder-data) repository.
