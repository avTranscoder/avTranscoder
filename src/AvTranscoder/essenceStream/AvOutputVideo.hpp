#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_OUTPUT_VIDEO_HPP_

#include "IOutputEssence.hpp"

#include <AvTranscoder/codec/DataStream.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/essenceStructures/VideoFrame.hpp>

#include <AvTranscoder/Profile.hpp>

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
	bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame );

	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( DataStream& codedFrame );

	void setProfile( const Profile::ProfileDesc& desc, const avtranscoder::VideoFrameDesc& frameDesc );
	
	VideoCodec getVideoCodec() { return _codedDesc; }
};

}

#endif