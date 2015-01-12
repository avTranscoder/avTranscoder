%apply char * { unsigned char * };

%{
#include <AvTranscoder/frame/Frame.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>
%}

namespace std {
%template(DataBuffer) std::vector< unsigned char >;
}

%include <AvTranscoder/frame/Frame.hpp>
%include <AvTranscoder/frame/VideoFrame.hpp>
%include <AvTranscoder/frame/AudioFrame.hpp>
