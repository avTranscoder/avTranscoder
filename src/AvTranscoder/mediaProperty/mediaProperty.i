%{
#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/mediaProperty/FileProperties.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/mediaProperty/DataProperties.hpp>
#include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
#include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
#include <AvTranscoder/mediaProperty/UnknownProperties.hpp>

using namespace avtranscoder;
%}

namespace std {
%template(VideoVector)        vector< avtranscoder::VideoProperties >;
%template(AudioVector)        vector< avtranscoder::AudioProperties >;
%template(DataVector)         vector< avtranscoder::DataProperties >;
%template(SubtitleVector)     vector< avtranscoder::SubtitleProperties >;
%template(AttachementVector)  vector< avtranscoder::AttachementProperties >;
%template(UnknownVector)      vector< avtranscoder::UnknownProperties >;

%template(MetadataPair)    pair< string, string >;
%template(MetadatasVector) vector< pair< string, string > >;

%template(GopPair)         pair< char, bool >;
%template(GopVector)       vector< pair< char, bool > >;

%template(ChannelVector)   vector< avtranscoder::Channel >;
}

%include <AvTranscoder/mediaProperty/util.hpp>
%include <AvTranscoder/mediaProperty/FileProperties.hpp>
%include <AvTranscoder/mediaProperty/VideoProperties.hpp>
%include <AvTranscoder/mediaProperty/AudioProperties.hpp>
%include <AvTranscoder/mediaProperty/DataProperties.hpp>
%include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
%include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
%include <AvTranscoder/mediaProperty/UnknownProperties.hpp>
