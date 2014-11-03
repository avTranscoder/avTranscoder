#ifndef _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_AV_INPUT_VIDEO_HPP_

#include "IInputEssence.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/Profile.hpp>

struct AVFrame;

namespace avtranscoder
{

class AvInputStream;

class AvExport AvInputVideo : public IInputEssence
{
public:
	AvInputVideo( AvInputStream& inputStream );
	~AvInputVideo();
	
	void setup();

	bool readNextFrame( Frame& frameBuffer );
	bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

	void flushDecoder();
	
	void setProfile( const Profile::ProfileDesc& desc );
	
private:
	bool decodeNextFrame();

	AvInputStream* _inputStream;  ///< Stream from which we read next frames
	const VideoCodec* _codec;  ///< Video decoder. Has link (no ownership)
	AVFrame* _frame;  ///< Libav object to store decoded data

	int _selectedStream;  ///< Index of the selected stream in the input file
};

}

#endif
