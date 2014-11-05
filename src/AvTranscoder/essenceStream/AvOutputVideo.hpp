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

	/**
	 * @param[out] codedFrame blabla
	 */
	bool encodeFrame( const Frame& sourceFrame, Frame& codedFrame );

	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( Frame& codedFrame );

	void setProfile( const ProfileLoader::Profile& profile, const avtranscoder::VideoFrameDesc& frameDesc );
	
	ICodec& getCodec() { return _codec; }
	VideoCodec& getVideoCodec() { return _codec; }

private:
	VideoCodec _codec;
};

}

#endif