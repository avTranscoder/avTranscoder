# Compilation

AvTranscoder uses CMake as build system.

#### Dependencies
AvTranscoder can depend on ffmpeg, libav, or any fork of these projects that follow the same API.
* Recommended ffmpeg versions: 2.2.2 or upper
* Recommended libav versions: not tested

#### To build
```
mkdir build && cd build
cmake ..
make
make install
```

#### To indicate a specific ffmpeg/libav folder
```
cmake .. -DCMAKE_PREFIX_PATH=/path/to/your/install
```

#### To not build apps
```
cmake .. -DAVTRANSCODER_DISABLE_APPS=True
```

#### To not build bindings
```
cmake .. -DAVTRANSCODER_DISABLE_BINDINGS=True
```
```
cmake .. -DAVTRANSCODER_DISABLE_PYTHON_BINDING=True
```
```
cmake .. -DAVTRANSCODER_DISABLE_JAVA_BINDING=True
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

#### Mac OSX using homebrew

###### Install homebrew
http://brew.sh/

###### Install avTranscoder
Last stable version:
```
brew tap cbenhagen/video
brew install avtranscoder
```

From develop branch:
```
brew tap cbenhagen/video
brew install avtranscoder --devel
```

###### Use homebrew to install only dependencies
```
brew deps avtranscoder
brew install avtranscoder --only-dependencies
```
