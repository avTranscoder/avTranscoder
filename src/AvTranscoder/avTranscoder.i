%module AvTranscoder

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"


%include "AvTranscoder/swig/avException.i"
%include "AvTranscoder/swig/avExport.i"

%{
#include <AvTranscoder/common.hpp>

#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/EssenceStructures/Pixel.hpp>
#include <AvTranscoder/EssenceStructures/Frame.hpp>
#include <AvTranscoder/EssenceStructures/VideoFrame.hpp>
#include <AvTranscoder/EssenceStructures/AudioFrame.hpp>

#include <AvTranscoder/CodedStructures/CodedDesc.hpp>
#include <AvTranscoder/CodedStructures/VideoDesc.hpp>
#include <AvTranscoder/CodedStructures/AudioDesc.hpp>
#include <AvTranscoder/CodedStructures/DataDesc.hpp>
#include <AvTranscoder/CodedStructures/DataStream.hpp>

#include <AvTranscoder/mediaProperty/mediaProperties.hpp>

#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/codedStream/IOutputStream.hpp>
#include <AvTranscoder/codedStream/AvOutputStream.hpp>

#include <AvTranscoder/codedStream/IInputStream.hpp>
#include <AvTranscoder/codedStream/AvInputStream.hpp>

#include <AvTranscoder/essenceStream/OutputEssence.hpp>
#include <AvTranscoder/essenceStream/OutputAudio.hpp>
#include <AvTranscoder/essenceStream/OutputVideo.hpp>

#include <AvTranscoder/essenceStream/InputEssence.hpp>
#include <AvTranscoder/essenceStream/InputAudio.hpp>
#include <AvTranscoder/essenceStream/InputVideo.hpp>

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/file/OutputFile.hpp>

#include <AvTranscoder/transcoder/StreamTranscoder.hpp>
#include <AvTranscoder/transcoder/Transcoder.hpp>
%}

%include "AvTranscoder/ProgressListener.i"

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

%include <AvTranscoder/common.hpp>

%include <AvTranscoder/Profile.hpp>

%include <AvTranscoder/EssenceStructures/Pixel.hpp>
%include <AvTranscoder/EssenceStructures/Frame.hpp>
%include <AvTranscoder/EssenceStructures/VideoFrame.hpp>
%include <AvTranscoder/EssenceStructures/AudioFrame.hpp>

%include <AvTranscoder/CodedStructures/CodedDesc.hpp>
%include <AvTranscoder/CodedStructures/VideoDesc.hpp>
%include <AvTranscoder/CodedStructures/AudioDesc.hpp>
%include <AvTranscoder/CodedStructures/DataDesc.hpp>
%include <AvTranscoder/CodedStructures/DataStream.hpp>


%include <AvTranscoder/mediaProperty/mediaProperties.hpp>

%include <AvTranscoder/Profile.hpp>

%include <AvTranscoder/codedStream/IOutputStream.hpp>
%include <AvTranscoder/codedStream/AvOutputStream.hpp>

%include <AvTranscoder/codedStream/IInputStream.hpp>
%include <AvTranscoder/codedStream/AvInputStream.hpp>

%include <AvTranscoder/essenceStream/OutputEssence.hpp>
%include <AvTranscoder/essenceStream/OutputAudio.hpp>
%include <AvTranscoder/essenceStream/OutputVideo.hpp>

%include <AvTranscoder/essenceStream/InputEssence.hpp>
%include <AvTranscoder/essenceStream/InputAudio.hpp>
%include <AvTranscoder/essenceStream/InputVideo.hpp>

%include <AvTranscoder/file/InputFile.hpp>
%include <AvTranscoder/file/OutputFile.hpp>

%include <AvTranscoder/transcoder/StreamTranscoder.hpp>
%include <AvTranscoder/transcoder/Transcoder.hpp>
