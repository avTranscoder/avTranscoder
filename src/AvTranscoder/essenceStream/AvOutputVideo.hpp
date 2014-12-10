#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_VIDEO_HPP_

#include "IOutputEssence.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport AvOutputVideo : public IOutputEssence
{
public:
	AvOutputVideo();

	void setup();

	bool encodeFrame( const Frame& sourceFrame, Frame& codedFrame );
	bool encodeFrame( Frame& codedFrame );  ///< Encode delayed encoded frames

	
	ICodec& getCodec() { return _codec; }
	VideoCodec& getVideoCodec() { return _codec; }

	void setProfile( const ProfileLoader::Profile& profile, const avtranscoder::VideoFrameDesc& frameDesc );
	void setVerbose( bool verbose = true ) { _verbose = verbose; }

private:
	VideoCodec _codec;

	bool _verbose;
};

}

#endif