%module(directors="1") mediaIO

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%include "mediaCore/swig/mediaException.i"
%include "mediaCore/swig/mediaExport.i"
%include "mediaCore/swig/mediaRational.i"

%{
#include <mediaIO/codec/ICodec.hpp>
#include <mediaIO/codec/VideoCodec.hpp>
#include <mediaIO/codec/AudioCodec.hpp>
#include <mediaIO/codec/DataCodec.hpp>

#include <mediaIO/mediaProperty/mediaProperty.hpp>

#include <mediaIO/codedStream/IOutputStream.hpp>
#include <mediaIO/codedStream/AvOutputStream.hpp>

#include <mediaIO/codedStream/IInputStream.hpp>
#include <mediaIO/codedStream/AvInputStream.hpp>

#include <mediaIO/essenceStream/IOutputEssence.hpp>
#include <mediaIO/essenceStream/AvOutputAudio.hpp>
#include <mediaIO/essenceStream/AvOutputVideo.hpp>

#include <mediaIO/essenceStream/IInputEssence.hpp>
#include <mediaIO/essenceStream/AvInputAudio.hpp>
#include <mediaIO/essenceStream/AvInputVideo.hpp>

#include <mediaIO/file/InputFile.hpp>
#include <mediaIO/file/OutputFile.hpp>

#include <mediaIO/transcoder/StreamTranscoder.hpp>
#include <mediaIO/transcoder/Transcoder.hpp>
%}

namespace std {
%template(IntPair)         pair< size_t, size_t >;
%template(VideoVector)     vector< mediaengine::mediaio::VideoProperties >;
%template(AudioVector)     vector< mediaengine::mediaio::AudioProperties >;
%template(MetadataPair)    pair< string, string >;
%template(MetadatasVector) vector< pair< string, string > >;
%template(GopPair)         pair< char, bool >;
%template(GopVector)       vector< pair< char, bool > >;
%template(ChannelVector)   vector< mediaengine::mediaio::Channel >;
}

%include <mediaCore/common.hpp>

%include <mediaIO/codec/ICodec.hpp>
%include <mediaIO/codec/VideoCodec.hpp>
%include <mediaIO/codec/AudioCodec.hpp>
%include <mediaIO/codec/DataCodec.hpp>

%include <mediaIO/mediaProperty/mediaProperty.hpp>

%include <mediaIO/codedStream/IOutputStream.hpp>
%include <mediaIO/codedStream/AvOutputStream.hpp>

%include <mediaIO/codedStream/IInputStream.hpp>
%include <mediaIO/codedStream/AvInputStream.hpp>

%include <mediaIO/essenceStream/IOutputEssence.hpp>
%include <mediaIO/essenceStream/AvOutputAudio.hpp>
%include <mediaIO/essenceStream/AvOutputVideo.hpp>

%include <mediaIO/essenceStream/IInputEssence.hpp>
%include <mediaIO/essenceStream/AvInputAudio.hpp>
%include <mediaIO/essenceStream/AvInputVideo.hpp>

%include <mediaIO/file/InputFile.hpp>
%include <mediaIO/file/OutputFile.hpp>

%include <mediaIO/transcoder/StreamTranscoder.hpp>
%include <mediaIO/transcoder/Transcoder.hpp>
