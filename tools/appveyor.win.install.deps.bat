set FFMPEG_VERSION=2.2.11

:: Installing ffmpeg dev (include + apps)
curl -kLO http://ffmpeg.zeranoe.com/builds/win32/dev/ffmpeg-%FFMPEG_VERSION%-win32-dev.7z
7z x ffmpeg-%FFMPEG_VERSION%-win32-dev.7z

:: Installing ffmpeg shared (libs)
curl -kLO http://ffmpeg.zeranoe.com/builds/win32/shared/ffmpeg-%FFMPEG_VERSION%-win32-shared.7z
7z x ffmpeg-%FFMPEG_VERSION%-win32-shared.7z

move ffmpeg-%FFMPEG_VERSION%-win32-shared\bin ffmpeg-%FFMPEG_VERSION%-win32-dev
