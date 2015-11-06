@echo on

set FFMPEG_VERSION=2.2.11

if %platform% == x86 set PLATFORM_VERSION=32
if %platform% == X64 set PLATFORM_VERSION=64

MKDIR build
cd build

:: Configure
call cmake.exe .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CD% -DCMAKE_PREFIX_PATH=C:\ProgramData\ffmpeg-%FFMPEG_VERSION%-win%PLATFORM_VERSION%-dev -DAVTRANSCODER_PYTHON_VERSION_OF_BINDING=2.7

:: Build & Install
call nmake /F Makefile
call nmake /F Makefile install

@echo off
