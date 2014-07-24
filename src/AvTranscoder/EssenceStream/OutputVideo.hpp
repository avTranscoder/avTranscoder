#ifndef _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_OUTPUT_VIDEO_HPP_

#include "OutputEssence.hpp"

#include <AvTranscoder/CodedStructures/DataStreamDesc.hpp>
#include <AvTranscoder/CodedStructures/VideoDesc.hpp>
#include <AvTranscoder/EssenceStructures/VideoFrame.hpp>

#include <AvTranscoder/Profile.hpp>

namespace avtranscoder
{

class AvExport OutputVideo : public OutputEssence
{
public:
	OutputVideo();

	void setup();

	//void setVideoDesc( const VideoDesc& videoDesc );

	/**
	 * @param[out] codedFrame blabla
	 */
	bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame );

	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( DataStream& codedFrame );

	void setProfile( const Profile::ProfileDesc& desc, const avtranscoder::VideoFrameDesc& videoFrameDesc );
	
	VideoDesc& getVideoDesc() { return _videoDesc; }

private:
	VideoDesc _videoDesc;
};

}

#endif