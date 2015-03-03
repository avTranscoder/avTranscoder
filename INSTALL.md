# Compilation

AvTranscoder uses CMake as build system.

#### To build
```
mkdir build && cd build
cmake ..
make
make install
```

#### To indicate a specific ffmpeg/libav folder
```
cmake .. -DFFMPEG_INCLUDE_DIR=/path/to/include/ -DFFMPEG_LIBRARY_DIR=/path/to/lib/
```

#### To not build apps
```
cmake .. -DDISABLE_APPS=True
```

#### To not build bindings
```
cmake .. -DDISABLE_BINDINGS=True
```
```
cmake .. -DDISABLE_PYTHON_BINDING=True
```
```
cmake .. -DDISABLE_JAVA_BINDING=True
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
