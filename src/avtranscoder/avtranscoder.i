%module avtranscoder

%module(directors="1") avtranscoder

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"

%include "avtranscoder/swig/avException.i"
%include "avtranscoder/swig/avExport.i"

%{
#include <avtranscoder/Profile.hpp>

#include <avtranscoder/frame/Pixel.hpp>
#include <avtranscoder/frame/Frame.hpp>
#include <avtranscoder/frame/VideoFrame.hpp>
#include <avtranscoder/frame/AudioFrame.hpp>

#include <avtranscoder/codec/ICodec.hpp>
#include <avtranscoder/codec/VideoCodec.hpp>
#include <avtranscoder/codec/AudioCodec.hpp>
#include <avtranscoder/codec/DataCodec.hpp>

#include <avtranscoder/mediaProperty/mediaProperty.hpp>

#include <avtranscoder/Profile.hpp>

#include <avtranscoder/codedStream/IOutputStream.hpp>
#include <avtranscoder/codedStream/AvOutputStream.hpp>

#include <avtranscoder/codedStream/IInputStream.hpp>
#include <avtranscoder/codedStream/AvInputStream.hpp>

#include <avtranscoder/essenceStream/IOutputEssence.hpp>
#include <avtranscoder/essenceStream/AvOutputAudio.hpp>
#include <avtranscoder/essenceStream/AvOutputVideo.hpp>

#include <avtranscoder/essenceStream/IInputEssence.hpp>
#include <avtranscoder/essenceStream/AvInputAudio.hpp>
#include <avtranscoder/essenceStream/AvInputVideo.hpp>

#include <avtranscoder/file/InputFile.hpp>
#include <avtranscoder/file/OutputFile.hpp>

#include <avtranscoder/transcoder/StreamTranscoder.hpp>
#include <avtranscoder/transcoder/Transcoder.hpp>
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

%include "avtranscoder/progress/progress.i"

%include <avtranscoder/Profile.hpp>

%include <avtranscoder/frame/Pixel.hpp>
%include <avtranscoder/frame/Frame.hpp>
%include <avtranscoder/frame/VideoFrame.hpp>
%include <avtranscoder/frame/AudioFrame.hpp>

%include <avtranscoder/codec/ICodec.hpp>
%include <avtranscoder/codec/VideoCodec.hpp>
%include <avtranscoder/codec/AudioCodec.hpp>
%include <avtranscoder/codec/DataCodec.hpp>

%include <avtranscoder/mediaProperty/mediaProperty.hpp>

%include <avtranscoder/Profile.hpp>

%include <avtranscoder/codedStream/IOutputStream.hpp>
%include <avtranscoder/codedStream/AvOutputStream.hpp>

%include <avtranscoder/codedStream/IInputStream.hpp>
%include <avtranscoder/codedStream/AvInputStream.hpp>

%include <avtranscoder/essenceStream/IOutputEssence.hpp>
%include <avtranscoder/essenceStream/AvOutputAudio.hpp>
%include <avtranscoder/essenceStream/AvOutputVideo.hpp>

%include <avtranscoder/essenceStream/IInputEssence.hpp>
%include <avtranscoder/essenceStream/AvInputAudio.hpp>
%include <avtranscoder/essenceStream/AvInputVideo.hpp>

%include <avtranscoder/file/InputFile.hpp>
%include <avtranscoder/file/OutputFile.hpp>

%include <avtranscoder/transcoder/StreamTranscoder.hpp>
%include <avtranscoder/transcoder/Transcoder.hpp>
