%module avtranscoder

%module(directors="1") avtranscoder

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%include "AvTranscoder/swig/avException.i"
%include "AvTranscoder/swig/avExport.i"
%include "AvTranscoder/swig/avDocumentation.i"
%include "AvTranscoder/swig/avRational.i"

%{
#include <AvTranscoder/ProfileLoader.hpp>

#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/DataCodec.hpp>

#include <AvTranscoder/codedStream/IOutputStream.hpp>
#include <AvTranscoder/codedStream/AvOutputStream.hpp>

#include <AvTranscoder/codedStream/IInputStream.hpp>
#include <AvTranscoder/codedStream/AvInputStream.hpp>

#include <AvTranscoder/essenceStream/IEncoder.hpp>
#include <AvTranscoder/essenceStream/AudioEncoder.hpp>
#include <AvTranscoder/essenceStream/VideoEncoder.hpp>

#include <AvTranscoder/essenceStream/IDecoder.hpp>
#include <AvTranscoder/essenceStream/AudioDecoder.hpp>
#include <AvTranscoder/essenceStream/VideoDecoder.hpp>
#include <AvTranscoder/essenceStream/VideoGenerator.hpp>
#include <AvTranscoder/essenceStream/AudioGenerator.hpp>

#include <AvTranscoder/transcoder/StreamTranscoder.hpp>
#include <AvTranscoder/transcoder/Transcoder.hpp>
%}

namespace std {
%template(IntPair)         pair< size_t, size_t >;
%template(ProfileMap)      map< string, string >;
%template(ProfilesVector)  vector< map< string, string > >;
%template(DataBuffer)      std::vector< unsigned char >;
}

%include "AvTranscoder/progress/progress.i"
%include "AvTranscoder/mediaProperty/mediaProperty.i"
%include "AvTranscoder/frame/frame.i"

%include <AvTranscoder/ProfileLoader.hpp>

%include <AvTranscoder/codec/ICodec.hpp>
%include <AvTranscoder/codec/VideoCodec.hpp>
%include <AvTranscoder/codec/AudioCodec.hpp>
%include <AvTranscoder/codec/DataCodec.hpp>

%include <AvTranscoder/codedStream/IOutputStream.hpp>
%include <AvTranscoder/codedStream/AvOutputStream.hpp>

%include <AvTranscoder/codedStream/IInputStream.hpp>
%include <AvTranscoder/codedStream/AvInputStream.hpp>

%include <AvTranscoder/essenceStream/IEncoder.hpp>
%include <AvTranscoder/essenceStream/AudioEncoder.hpp>
%include <AvTranscoder/essenceStream/VideoEncoder.hpp>

%include <AvTranscoder/essenceStream/IDecoder.hpp>
%include <AvTranscoder/essenceStream/AudioDecoder.hpp>
%include <AvTranscoder/essenceStream/VideoDecoder.hpp>
%include <AvTranscoder/essenceStream/VideoGenerator.hpp>
%include <AvTranscoder/essenceStream/AudioGenerator.hpp>

%include "AvTranscoder/file/file.i"

%include <AvTranscoder/transcoder/StreamTranscoder.hpp>
%include <AvTranscoder/transcoder/Transcoder.hpp>
