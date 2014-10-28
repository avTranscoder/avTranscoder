#ifndef _MEDIA_MANAGER_IO_ESSENCE_STREAM_AV_OUTPUT_AUDIO_HPP_
#define _MEDIA_MANAGER_IO_ESSENCE_STREAM_AV_OUTPUT_AUDIO_HPP_

#include "IOutputEssence.hpp"
#include <mediaCore/profile/Profile.hpp>
#include <mediaIO/codec/AudioCodec.hpp>

namespace mediamanager
{
namespace mediaio
{

class MediaManagerExport AvOutputAudio : public IOutputEssence
{
public:
	AvOutputAudio();
	
	void setup();

	/**
	 * @param[out] codedFrame
	 */
	bool encodeFrame( const mediacore::Frame& sourceFrame, mediacore::Frame& codedFrame );
	
	/**
	 * get delayed encoded frames
	 */
	bool encodeFrame( mediacore::Frame& codedFrame );
	
	void setProfile( const mediacore::Profile::ProfileDesc& desc, const mediacore::AudioFrameDesc& frameDesc );

	ICodec& getCodec() { return _codec; }
	AudioCodec& getAudioCodec() { return _codec; }

private:
	AudioCodec _codec;
};

}
}

#endif