%module AvTranscoder

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"


%include "AvTranscoder/avException.i"

%{
#include <AvTranscoder/common.hpp>

#include <AvTranscoder/DatasStructures/Pixel.hpp>
#include <AvTranscoder/DatasStructures/Frame.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>
#include <AvTranscoder/DatasStructures/AudioFrame.hpp>
#include <AvTranscoder/DatasStructures/VideoDesc.hpp>
#include <AvTranscoder/DatasStructures/AudioDesc.hpp>
#include <AvTranscoder/DatasStructures/DataStreamDesc.hpp>

#include <AvTranscoder/Metadatas/MediaMetadatasStructures.hpp>

#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/CodedStream/OutputStream.hpp>
#include <AvTranscoder/CodedStream/AvOutputStream.hpp>

#include <AvTranscoder/CodedStream/InputStream.hpp>
#include <AvTranscoder/CodedStream/AvInputStream.hpp>

#include <AvTranscoder/EssenceStream/OutputEssence.hpp>
#include <AvTranscoder/EssenceStream/OutputAudio.hpp>
#include <AvTranscoder/EssenceStream/OutputVideo.hpp>

#include <AvTranscoder/EssenceStream/InputEssence.hpp>
#include <AvTranscoder/EssenceStream/InputAudio.hpp>
#include <AvTranscoder/EssenceStream/InputVideo.hpp>

#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/File/OutputFile.hpp>

#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/Transcoder/StreamTranscoder.hpp>
#include <AvTranscoder/Transcoder/Transcoder.hpp>


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
}

%include <AvTranscoder/common.hpp>

%include <AvTranscoder/DatasStructures/Pixel.hpp>
%include <AvTranscoder/DatasStructures/Frame.hpp>
%include <AvTranscoder/DatasStructures/Image.hpp>
%include <AvTranscoder/DatasStructures/AudioFrame.hpp>
%include <AvTranscoder/DatasStructures/VideoDesc.hpp>
%include <AvTranscoder/DatasStructures/AudioDesc.hpp>
%include <AvTranscoder/DatasStructures/DataStreamDesc.hpp>

%include <AvTranscoder/Metadatas/MediaMetadatasStructures.hpp>

%include <AvTranscoder/Profile.hpp>

%include <AvTranscoder/CodedStream/OutputStream.hpp>
%include <AvTranscoder/CodedStream/AvOutputStream.hpp>

%include <AvTranscoder/CodedStream/InputStream.hpp>
%include <AvTranscoder/CodedStream/AvInputStream.hpp>

%include <AvTranscoder/EssenceStream/OutputEssence.hpp>
%include <AvTranscoder/EssenceStream/OutputAudio.hpp>
%include <AvTranscoder/EssenceStream/OutputVideo.hpp>

%include <AvTranscoder/EssenceStream/InputEssence.hpp>
%include <AvTranscoder/EssenceStream/InputAudio.hpp>
%include <AvTranscoder/EssenceStream/InputVideo.hpp>

%include <AvTranscoder/File/InputFile.hpp>
%include <AvTranscoder/File/OutputFile.hpp>

%include <AvTranscoder/Profile.hpp>

%include <AvTranscoder/Transcoder/StreamTranscoder.hpp>
%include <AvTranscoder/Transcoder/Transcoder.hpp>

