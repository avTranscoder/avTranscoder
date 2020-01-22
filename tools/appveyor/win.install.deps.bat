@echo on

if %platform% == x86 set PLATFORM_VERSION=32
if %platform% == X64 set PLATFORM_VERSION=64

:: Installing ffmpeg dev (include + apps)
curl -kLO http://ffmpeg.zeranoe.com/builds/win%PLATFORM_VERSION%/dev/ffmpeg-%DEPENDENCY_VERSION%-win%PLATFORM_VERSION%-dev.7z
7z x ffmpeg-%DEPENDENCY_VERSION%-win%PLATFORM_VERSION%-dev.zip

:: Installing ffmpeg shared (libs)
curl -kLO http://ffmpeg.zeranoe.com/builds/win%PLATFORM_VERSION%/shared/ffmpeg-%DEPENDENCY_VERSION%-win%PLATFORM_VERSION%-shared.7z
7z x ffmpeg-%DEPENDENCY_VERSION%-win%PLATFORM_VERSION%-shared.zip

move ffmpeg-%DEPENDENCY_VERSION%-win%PLATFORM_VERSION%-shared\bin ffmpeg-%DEPENDENCY_VERSION%-win%PLATFORM_VERSION%-dev
move ffmpeg-%DEPENDENCY_VERSION%-win%PLATFORM_VERSION%-dev %DEPENDENCY_INSTALL_PATH%

@echo off
