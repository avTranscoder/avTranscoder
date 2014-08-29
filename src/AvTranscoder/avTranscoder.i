%module AvTranscoder

%include "std_string.i"
%include "std_vector.i"
%include "std_pair.i"
%include "std_map.i"


%include "AvTranscoder/avException.i"

// Need to modify the visibility of the constructor and getCPtr method
// if you are invoking SWIG more than once and generating the wrapped classes
// into different packages in each invocation.
%typemap(javabody) SWIGTYPE %{
protected long swigCPtr;
protected boolean swigCMemOwn;

public $javaclassname(long cPtr, boolean cMemoryOwn) {
	swigCMemOwn = cMemoryOwn;
	swigCPtr = cPtr;
}
public static long getCPtr($javaclassname obj) {
	return (obj == null) ? 0 : obj.swigCPtr;
}
%}
%typemap(javabody_derived) SWIGTYPE %{
public $javaclassname(long cPtr, boolean cMemoryOwn) {
	super( cPtr, cMemoryOwn );
}
public static long getCPtr($javaclassname obj) {
	return (obj == null) ? 0 : obj.swigCPtr;
}
%}

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

%include <AvTranscoder/Transcoder/StreamTranscoder.hpp>
%include <AvTranscoder/Transcoder/Transcoder.hpp>

