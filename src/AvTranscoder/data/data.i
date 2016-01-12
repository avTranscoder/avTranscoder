%apply char * { unsigned char * };

%{
#include <AvTranscoder/data/CodedData.hpp>
#include <AvTranscoder/data/Frame.hpp>
#include <AvTranscoder/data/VideoFrame.hpp>
#include <AvTranscoder/data/AudioFrame.hpp>
%}

%include <AvTranscoder/data/CodedData.hpp>
%include <AvTranscoder/data/Frame.hpp>
%include <AvTranscoder/data/VideoFrame.hpp>
%include <AvTranscoder/data/AudioFrame.hpp>
