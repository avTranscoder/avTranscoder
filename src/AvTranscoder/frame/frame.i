%apply char * { unsigned char * };

%{
#include <AvTranscoder/frame/Pixel.hpp>
#include <AvTranscoder/frame/Frame.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>
%}

%include <AvTranscoder/frame/Pixel.hpp>
%include <AvTranscoder/frame/Frame.hpp>
%include <AvTranscoder/frame/VideoFrame.hpp>
%include <AvTranscoder/frame/AudioFrame.hpp>
