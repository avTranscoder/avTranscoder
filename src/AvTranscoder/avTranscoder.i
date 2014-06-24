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

#include <AvTranscoder/OutputStream.hpp>
#include <AvTranscoder/AvOutputStream.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>

#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/AvInputStream.hpp>
#include <AvTranscoder/InputStreamReader.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/InputStreamVideo.hpp>

#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/OutputFile.hpp>

#include <AvTranscoder/Transcoder.hpp>

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

%include <AvTranscoder/OutputStream.hpp>
%include <AvTranscoder/AvOutputStream.hpp>
%include <AvTranscoder/OutputStreamAudio.hpp>
%include <AvTranscoder/OutputStreamVideo.hpp>

%include <AvTranscoder/InputStream.hpp>
%include <AvTranscoder/AvInputStream.hpp>
%include <AvTranscoder/InputStreamReader.hpp>
%include <AvTranscoder/InputStreamAudio.hpp>
%include <AvTranscoder/InputStreamVideo.hpp>

%include <AvTranscoder/InputFile.hpp>
%include <AvTranscoder/OutputFile.hpp>

%include <AvTranscoder/Transcoder.hpp>
