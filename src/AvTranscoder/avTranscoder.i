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
%include "AvTranscoder/swig/avSeek.i"
%include "AvTranscoder/swig/avOperator.i"

%{
#include <AvTranscoder/log.hpp>
%}

%include "AvTranscoder/progress/progress.i"
%include "AvTranscoder/properties/properties.i"
%include "AvTranscoder/data/data.i"
%include "AvTranscoder/profile/profile.i"

%include <AvTranscoder/log.hpp>

%include "AvTranscoder/library.i"
%include "AvTranscoder/option.i"
%include "AvTranscoder/util.i"
%include "AvTranscoder/codec/codec.i"
%include "AvTranscoder/stream/stream.i"
%include "AvTranscoder/decoder/decoder.i"
%include "AvTranscoder/encoder/encoder.i"
%include "AvTranscoder/transform/transform.i"
%include "AvTranscoder/file/file.i"
%include "AvTranscoder/stat/stat.i"
%include "AvTranscoder/filter/filter.i"
%include "AvTranscoder/transcoder/transcoder.i"
%include "AvTranscoder/reader/reader.i"
