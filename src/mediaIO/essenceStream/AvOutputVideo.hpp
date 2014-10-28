#ifndef _MEDIA_MANAGER_IO_ESSENCE_STREAM_AV_OUTPUT_VIDEO_HPP_
#define _MEDIA_MANAGER_IO_ESSENCE_STREAM_AV_OUTPUT_VIDEO_HPP_

#include "IOutputEssence.hpp"
#include <mediaCore/profile/Profile.hpp>
#include <mediaIO/codec/VideoCodec.hpp>

namespace mediamanager
{
namespace mediaio
{

class MediaManagerExport AvOutputVideo : public IOutputEssence
{
public:
	AvOutputVideo();

	void setup();

	/**
	 * @param[out] codedFrame blabla
	 */
	bool encodeFrame( const mediacore::Frame& sourceFrame, mediacore::Frame& codedFrame );

	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( mediacore::Frame& codedFrame );

	void setProfile( const mediacore::Profile::ProfileDesc& desc, const mediacore::VideoFrameDesc& frameDesc );
	
	ICodec& getCodec() { return _codec; }
	VideoCodec& getVideoCodec() { return _codec; }

private:
	VideoCodec _codec;
};

}
}

#endif