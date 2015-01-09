#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_VIDEO_HPP_

#include "IEncoder.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport VideoEncoder : public IEncoder
{
public:
	VideoEncoder( const std::string& videoCodecName );

	void setup();

	bool encodeFrame( const Frame& sourceFrame, Frame& codedFrame );
	bool encodeFrame( Frame& codedFrame );
	
	ICodec& getCodec() { return _codec; }
	VideoCodec& getVideoCodec() { return _codec; }

	void setProfile( const ProfileLoader::Profile& profile, const avtranscoder::VideoFrameDesc& frameDesc );

private:
	VideoCodec _codec;
};

}

#endif