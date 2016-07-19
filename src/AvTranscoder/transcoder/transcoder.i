%{
#include <AvTranscoder/transcoder/InputStreamDesc.hpp>
#include <AvTranscoder/transcoder/StreamTranscoder.hpp>
#include <AvTranscoder/transcoder/Transcoder.hpp>
%}

%template(StreamTranscoderVector) std::vector< avtranscoder::StreamTranscoder* >;

%include <AvTranscoder/transcoder/InputStreamDesc.hpp>
%include <AvTranscoder/transcoder/StreamTranscoder.hpp>
%include <AvTranscoder/transcoder/Transcoder.hpp>
