%{
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/mediaProperty/FileProperties.hpp>
#include <AvTranscoder/mediaProperty/PixelProperties.hpp>
#include <AvTranscoder/mediaProperty/StreamProperties.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/mediaProperty/DataProperties.hpp>
#include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
#include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
#include <AvTranscoder/mediaProperty/UnknownProperties.hpp>

using namespace avtranscoder;
%}

namespace std {
// Allow vector of object with no default constructor
%ignore vector< avtranscoder::VideoProperties >::vector(size_type); 
%ignore vector< avtranscoder::VideoProperties >::resize;
%ignore vector< avtranscoder::AudioProperties >::vector(size_type); 
%ignore vector< avtranscoder::AudioProperties >::resize;
%ignore vector< avtranscoder::DataProperties >::vector(size_type); 
%ignore vector< avtranscoder::DataProperties >::resize;
%ignore vector< avtranscoder::SubtitleProperties >::vector(size_type); 
%ignore vector< avtranscoder::SubtitleProperties >::resize;
%ignore vector< avtranscoder::AttachementProperties >::vector(size_type); 
%ignore vector< avtranscoder::AttachementProperties >::resize;
%ignore vector< avtranscoder::UnknownProperties >::vector(size_type); 
%ignore vector< avtranscoder::UnknownProperties >::resize;

// Create instantiations of a template classes
%template(VideoVector)        vector< avtranscoder::VideoProperties >;
%template(AudioVector)        vector< avtranscoder::AudioProperties >;
%template(DataVector)         vector< avtranscoder::DataProperties >;
%template(SubtitleVector)     vector< avtranscoder::SubtitleProperties >;
%template(AttachementVector)  vector< avtranscoder::AttachementProperties >;
%template(UnknownVector)      vector< avtranscoder::UnknownProperties >;

%template(PropertyPair)    pair< string, string >;
%template(PropertyVector)  vector< pair< string, string > >;

%template(GopPair)         pair< char, bool >;
%template(GopVector)       vector< pair< char, bool > >;

%template(ChannelVector)   vector< avtranscoder::Channel >;
}

%include <AvTranscoder/mediaProperty/util.hpp>
%include <AvTranscoder/mediaProperty/FileProperties.hpp>
%include <AvTranscoder/mediaProperty/PixelProperties.hpp>
%include <AvTranscoder/mediaProperty/StreamProperties.hpp>
%include <AvTranscoder/mediaProperty/VideoProperties.hpp>
%include <AvTranscoder/mediaProperty/AudioProperties.hpp>
%include <AvTranscoder/mediaProperty/DataProperties.hpp>
%include <AvTranscoder/mediaProperty/SubtitleProperties.hpp>
%include <AvTranscoder/mediaProperty/AttachementProperties.hpp>
%include <AvTranscoder/mediaProperty/UnknownProperties.hpp>
