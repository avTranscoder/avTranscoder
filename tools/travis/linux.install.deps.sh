#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# Use TRAVIS_JOB_ID to detect that we are in travis.
# In that case, use a simple check to detect if the cache is already there.
if  [ -z ${TRAVIS_JOB_ID} ] || [ ! -d "${DEPENDENCY_INSTALL_PATH}/lib/" ]; then

    export LD_LIBRARY_PATH=${DEPENDENCY_INSTALL_PATH}/lib:${DEPENDENCY_INSTALL_PATH}/lib64
    export PKG_CONFIG_PATH=${DEPENDENCY_INSTALL_PATH}/lib/pkgconfig:${DEPENDENCY_INSTALL_PATH}/lib64/pkgconfig
    export PATH=$PATH:${DEPENDENCY_INSTALL_PATH}/bin

    # yasm
    echo "Building YASM (${YASM_VERSION})"
    DIR=$(mktemp -d yasmXXX) && cd ${DIR} && \
                  curl -Os http://www.tortall.net/projects/yasm/releases/yasm-${YASM_VERSION}.tar.gz && \
                  tar xzf yasm-${YASM_VERSION}.tar.gz && \
                  cd yasm-${YASM_VERSION} && \
                  ./configure --prefix="$DEPENDENCY_INSTALL_PATH" --bindir="${DEPENDENCY_INSTALL_PATH}/bin" && \
                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                  if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                  make install && \
                  rm -rf ${DIR}

    # x264
    echo ""
    echo "Building x264 (last version)"
    DIR=$(mktemp -d x264XXX) && cd ${DIR} && \
                  git clone --depth 1 git://git.videolan.org/x264 && \
                  cd x264 && \
                  ./configure --prefix="$DEPENDENCY_INSTALL_PATH" --bindir="${DEPENDENCY_INSTALL_PATH}/bin" --enable-shared --disable-asm && \
                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                  if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                  make install && \
                  rm -rf ${DIR}

    # libmp3lame
    echo ""
    echo "Building libmp3lame (${LAME_VERSION})"
    DIR=$(mktemp -d libmp3lameXXX) && cd ${DIR} && \
                  curl -L -Os http://downloads.sourceforge.net/project/lame/lame/${LAME_VERSION%.*}/lame-${LAME_VERSION}.tar.gz  && \
                  tar xzf lame-${LAME_VERSION}.tar.gz && \
                  cd lame-${LAME_VERSION} && \
                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" --bindir="${DEPENDENCY_INSTALL_PATH}/bin" --enable-nasm && \
                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                  if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                  make install && \
                  rm -rf ${DIR}


    # faac
    # Compilation error on OSX
    # http://stackoverflow.com/a/4320377
    # http://sourceforge.net/p/faac/bugs/162/#46a0
#    echo ""
#    echo "Building faac (${FAAC_VERSION})"
#    DIR=$(mktemp -d faacXXX) && cd ${DIR} && \
#                  curl -L -Os http://downloads.sourceforge.net/faac/faac-${FAAC_VERSION}.tar.gz  && \
#                  tar xzf faac-${FAAC_VERSION}.tar.gz && \
#                  cd faac-${FAAC_VERSION} && \
#                  sed -i '126d' common/mp4v2/mpeg4ip.h && \
#                  ./bootstrap && \
#                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" --bindir="${DEPENDENCY_INSTALL_PATH}/bin" --enable-shared --with-mp4v2=no && \
#                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
#                  make install && \
#                  rm -rf ${DIR}

    # xvid
    echo ""
    echo "Building xvid (${XVID_VERSION})"
    DIR=$(mktemp -d xvidXXX) && cd ${DIR} && \
                  curl -L -Os  http://downloads.xvid.org/downloads/xvidcore-${XVID_VERSION}.tar.gz  && \
                  tar xzf xvidcore-${XVID_VERSION}.tar.gz && \
                  cd xvidcore/build/generic && \
                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" --bindir="${DEPENDENCY_INSTALL_PATH}/bin" && \
                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                  if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                  make install && \
                  rm -rf ${DIR}


    # fdk-aac
    # Compilation error on OSX
    # Warning: need automake + libtool
#    echo ""
#    echo "Building fdk-aac (${FDKAAC_VERSION})"
#    DIR=$(mktemp -d fdk-aacXXX) && cd ${DIR} && \
#                  curl -s https://codeload.github.com/mstorsjo/fdk-aac/tar.gz/v${FDKAAC_VERSION} | tar zxf - && \
#                  cd fdk-aac-${FDKAAC_VERSION} && \
#                  autoreconf -fiv && \
#                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" --enable-shared && \
#                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
#                  make install && \
#                  rm -rf ${DIR}

    # libogg
    echo ""
    echo "Building libogg (${OGG_VERSION})"
    DIR=$(mktemp -d liboggXXX) && cd ${DIR} && \
                  wget http://downloads.xiph.org/releases/ogg/libogg-${OGG_VERSION}.tar.gz && \
                  tar xzf libogg-${OGG_VERSION}.tar.gz && \
                  cd libogg-${OGG_VERSION} && \
                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" --disable-shared --with-pic && \
                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                  if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                  make install && \
                  rm -rf ${DIR}

    # libvorbis
    echo ""
    echo "Building libvorbis (${VORBIS_VERSION})"
    DIR=$(mktemp -d libvorbisXXX) && cd ${DIR} && \
                  wget http://downloads.xiph.org/releases/vorbis/libvorbis-${VORBIS_VERSION}.tar.gz && \
                  tar xzf libvorbis-${VORBIS_VERSION}.tar.gz && \
                  cd libvorbis-${VORBIS_VERSION} && \
                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" --with-ogg="${DEPENDENCY_INSTALL_PATH}" --disable-shared --with-pic && \
                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                  if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                  make install && \
                  rm -rf ${DIR}

    # libtheora
    # Compilation error with clang
#    echo ""
#    echo "Building libtheora (${THEORA_VERSION})"
#    DIR=$(mktemp -d libtheoraXXX) && cd ${DIR} && \
#                  curl -O http://downloads.xiph.org/releases/theora/libtheora-${THEORA_VERSION}.tar.bz2 && \
#                  tar xvjf libtheora-${THEORA_VERSION}.tar.bz2 && \
#                  cd libtheora-${THEORA_VERSION} && \
#                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" && \
#                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
#                  make install && \
#                  make check && \
#                  rm -rf ${DIR}

    # libvpx
    # https://trac.ffmpeg.org/ticket/4956
    echo ""
    echo "Building libvpx (${VPX_VERSION})"
    DIR=$(mktemp -d libvpxXXX) && cd ${DIR} && \
                  git clone https://github.com/webmproject/libvpx.git && \
                  cd libvpx && \
                  git checkout v${VPX_VERSION} && \
                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" --disable-examples --enable-pic && \
                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                  if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                  make install && \
                  rm -rf ${DIR}

    # libopus
    # Compilation error on OSX
#    echo ""
#    echo "Building libopus (last version)"
#    DIR=$(mktemp -d libopusXXX) && cd ${DIR} && \
#                  git clone git://git.opus-codec.org/opus.git && \
#                  cd opus && \
#                  autoreconf -fiv && \
#                  ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" --enable-shared --with-pic && \
#                  make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
#                  make install && \
#                  rm -rf ${DIR}

    export COMPILE_OPTIONS=--extra-libs=-ldl\ --enable-small\ --enable-shared\ --disable-static
    export RELEASE_OPTIONS=--disable-debug
    export DEBUG_OPTIONS=--enable-debug=3\ --disable-optimizations\ --disable-sse\ --disable-stripping
    export LICENSING_OPTIONS=--enable-gpl\ --enable-nonfree
    export THIRD_PARTIES_OPTIONS=--enable-libmp3lame\ --enable-libx264\ --enable-libxvid\ --enable-avresample\ --enable-libvorbis\ --enable-libvpx

    if [[ ${DEPENDENCY_NAME} == "ffmpeg" ]]; then

        echo ""
        echo "Building ffmpeg (${DEPENDENCY_VERSION})"
        DIR=$(mktemp -d ffmpegXXX) && cd ${DIR} && \
                      curl -Os http://ffmpeg.org/releases/ffmpeg-${DEPENDENCY_VERSION}.tar.gz && \
                      tar xzf ffmpeg-${DEPENDENCY_VERSION}.tar.gz && \
                      cd ffmpeg-${DEPENDENCY_VERSION} && \
                      ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" \
                      --extra-cflags="-I${DEPENDENCY_INSTALL_PATH}/include" --extra-ldflags="-L${DEPENDENCY_INSTALL_PATH}/lib64 -L${DEPENDENCY_INSTALL_PATH}/lib" --bindir="${DEPENDENCY_INSTALL_PATH}/bin" \
                      $COMPILE_OPTIONS \
                      $RELEASE_OPTIONS \
                      $LICENSING_OPTIONS \
                      $THIRD_PARTIES_OPTIONS --enable-postproc && \
                      make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                      if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                      make install && \
                      rm -rf ${DIR}

    elif [[ ${DEPENDENCY_NAME} == "libav" ]]; then

        echo ""
        echo "Building libav (${DEPENDENCY_VERSION})"
        DIR=$(mktemp -d libavXXX) && cd ${DIR} && \
                      curl -Os https://libav.org/releases/libav-${DEPENDENCY_VERSION}.tar.gz && \
                      tar xzf libav-${DEPENDENCY_VERSION}.tar.gz && \
                      cd libav-${DEPENDENCY_VERSION} && \
                      ./configure --prefix="${DEPENDENCY_INSTALL_PATH}" \
                      --extra-cflags="-I${DEPENDENCY_INSTALL_PATH}/include" --extra-ldflags="-L${DEPENDENCY_INSTALL_PATH}/lib64 -L${DEPENDENCY_INSTALL_PATH}/lib" --bindir="${DEPENDENCY_INSTALL_PATH}/bin" \
                      $COMPILE_OPTIONS \
                      $RELEASE_OPTIONS \
                      $LICENSING_OPTIONS \
                      $THIRD_PARTIES_OPTIONS && \
                      make -k > ${DEPENDENCY_LOG_FILE} 2>&1 && \
                      if [ $? != 0 ]; then cat ${DEPENDENCY_LOG_FILE} && exit 1; fi
                      make install && \
                      rm -rf ${DIR}

    fi

fi
