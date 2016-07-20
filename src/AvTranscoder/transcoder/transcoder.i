%{
#include <AvTranscoder/transcoder/InputStreamDesc.hpp>
#include <AvTranscoder/transcoder/StreamTranscoder.hpp>
#include <AvTranscoder/transcoder/Transcoder.hpp>
%}

%template(StreamTranscoderVector) std::vector< avtranscoder::StreamTranscoder* >;
%template(InputStreamDescVector) std::vector< avtranscoder::InputStreamDesc >;

%include <AvTranscoder/transcoder/InputStreamDesc.hpp>
%include <AvTranscoder/transcoder/StreamTranscoder.hpp>
%include <AvTranscoder/transcoder/Transcoder.hpp>
