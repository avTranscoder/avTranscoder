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

#include <AvTranscoder/frame/Pixel.hpp>
#include <AvTranscoder/frame/Frame.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>

#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/DataCodec.hpp>

#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/mediaProperty/FileProperties.hpp>

#include <AvTranscoder/codedStream/IOutputStream.hpp>
#include <AvTranscoder/codedStream/AvOutputStream.hpp>

#include <AvTranscoder/codedStream/IInputStream.hpp>
#include <AvTranscoder/codedStream/AvInputStream.hpp>

#include <AvTranscoder/essenceStream/IOutputEssence.hpp>
#include <AvTranscoder/essenceStream/AvOutputAudio.hpp>
#include <AvTranscoder/essenceStream/AvOutputVideo.hpp>

#include <AvTranscoder/essenceStream/IInputEssence.hpp>
#include <AvTranscoder/essenceStream/AvInputAudio.hpp>
#include <AvTranscoder/essenceStream/AvInputVideo.hpp>

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/file/OutputFile.hpp>

#include <AvTranscoder/transcoder/StreamTranscoder.hpp>
#include <AvTranscoder/transcoder/Transcoder.hpp>

using namespace avtranscoder;
%}

namespace std {	
%template(IntPair)         pair< size_t, size_t >;
%template(VideoVector)     vector< avtranscoder::VideoProperties >;
%template(AudioVector)     vector< avtranscoder::AudioProperties >;
%template(MetadataPair)    pair< string, string >;
%template(MetadatasVector) vector< pair< string, string > >;
%template(GopPair)         pair< char, bool >;
%template(GopVector)       vector< pair< char, bool > >;
%template(ChannelVector)   vector< avtranscoder::Channel >;
%template(ProfileMap)      map< string, string >;
%template(ProfilesVector)  vector< map< string, string > >;
}

%include "AvTranscoder/progress/progress.i"

%include <AvTranscoder/ProfileLoader.hpp>

%include <AvTranscoder/frame/Pixel.hpp>
%include <AvTranscoder/frame/Frame.hpp>
%include <AvTranscoder/frame/VideoFrame.hpp>
%include <AvTranscoder/frame/AudioFrame.hpp>

%include <AvTranscoder/codec/ICodec.hpp>
%include <AvTranscoder/codec/VideoCodec.hpp>
%include <AvTranscoder/codec/AudioCodec.hpp>
%include <AvTranscoder/codec/DataCodec.hpp>

%include <AvTranscoder/mediaProperty/util.hpp>
%include <AvTranscoder/mediaProperty/FileProperties.hpp>

%include <AvTranscoder/codedStream/IOutputStream.hpp>
%include <AvTranscoder/codedStream/AvOutputStream.hpp>

%include <AvTranscoder/codedStream/IInputStream.hpp>
%include <AvTranscoder/codedStream/AvInputStream.hpp>

%include <AvTranscoder/essenceStream/IOutputEssence.hpp>
%include <AvTranscoder/essenceStream/AvOutputAudio.hpp>
%include <AvTranscoder/essenceStream/AvOutputVideo.hpp>

%include <AvTranscoder/essenceStream/IInputEssence.hpp>
%include <AvTranscoder/essenceStream/AvInputAudio.hpp>
%include <AvTranscoder/essenceStream/AvInputVideo.hpp>

%include <AvTranscoder/file/InputFile.hpp>
%include <AvTranscoder/file/OutputFile.hpp>

%include <AvTranscoder/transcoder/StreamTranscoder.hpp>
%include <AvTranscoder/transcoder/Transcoder.hpp>
