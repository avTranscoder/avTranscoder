%module avtranscoder

%module(directors="1") avtranscoder

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%include "AvTranscoder/swig/avException.i"
%include "AvTranscoder/swig/avExport.i"
%include "AvTranscoder/swig/avDocumentation.i"
%include "AvTranscoder/swig/avMediaType.i"
%include "AvTranscoder/swig/avRational.i"
%include "AvTranscoder/swig/avLogLevel.i"

%{
#include <AvTranscoder/Library.hpp>
#include <AvTranscoder/log.hpp>
%}

%include "AvTranscoder/progress/progress.i"
%include "AvTranscoder/mediaProperty/mediaProperty.i"
%include "AvTranscoder/frame/frame.i"
%include "AvTranscoder/profile/profile.i"

%include <AvTranscoder/Library.hpp>
%include <AvTranscoder/log.hpp>

%include "AvTranscoder/option.i"
%include "AvTranscoder/util.i"
%include "AvTranscoder/codec/codec.i"
%include "AvTranscoder/stream/stream.i"
%include "AvTranscoder/decoder/decoder.i"
%include "AvTranscoder/encoder/encoder.i"
%include "AvTranscoder/transform/transform.i"
%include "AvTranscoder/file/file.i"
%include "AvTranscoder/transcoder/transcoder.i"
