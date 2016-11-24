FROM gliderlabs/alpine:3.3

ADD . /src

RUN apk add --update cmake make g++ ffmpeg swig ffmpeg-dev doxygen python-dev freeglut freeglut-dev
RUN mkdir /src/build

WORKDIR /src/build

RUN cmake ..
RUN make
RUN make install
