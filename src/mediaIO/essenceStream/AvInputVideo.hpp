#ifndef _MEDIA_ENGINE_IO_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_
#define _MEDIA_ENGINE_IO_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_

#include "IInputEssence.hpp"
#include <mediaCore/profile/Profile.hpp>
#include <mediaIO/codec/VideoCodec.hpp>

struct AVFrame;

namespace mediaengine
{
namespace mediaio
{

class AvInputStream;

class MediaEngineExport AvInputVideo : public IInputEssence
{
public:
	AvInputVideo( AvInputStream& inputStream );
	~AvInputVideo();
	
	void setup();

	bool readNextFrame( mediacore::Frame& frameBuffer );
	bool readNextFrame( mediacore::Frame& frameBuffer, const size_t subStreamIndex );

	void flushDecoder();
	
	void setProfile( const mediacore::Profile::ProfileDesc& desc );
	
private:
	AvInputStream*     _inputStream;
	VideoCodec         _codec;
	AVFrame*           _frame;

	int                _selectedStream;
};

}
}

#endif
