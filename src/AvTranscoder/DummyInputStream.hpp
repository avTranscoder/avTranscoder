#ifndef _AV_TRANSCODER_DUMMY_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_DUMMY_INPUT_STREAM_HPP_

#include <AvTranscoder/CodedStream/InputStream.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>
#include <AvTranscoder/DatasStructures/AudioFrame.hpp>

namespace avtranscoder
{

class AvExport DummyInputStream : public InputStream
{
public:
	DummyInputStream( );

	~DummyInputStream( );

	size_t getStreamIndex() const { return 0; }

	bool readNextPacket( DataStream& data );

	// Stream properties
	void setVideoDesc( VideoDesc videoDesc );
	void setAudioDesc( AudioDesc audioDesc );

	void setImageDesc( ImageDesc& imageDesc ){ _imageDesc = imageDesc; }
	void setFrameDesc( AudioFrameDesc& frameDesc ){ _frameDesc = frameDesc; }

	VideoDesc getVideoDesc() const;
	AudioDesc getAudioDesc() const;

	AVMediaType getStreamType() const;

	double getDuration() const;
	double getPacketDuration() const;

	void setBufferred( const bool bufferized ){ }

	void clearBuffering();

private:
	VideoDesc _videoDesc;
	AudioDesc _audioDesc;

	ImageDesc      _imageDesc;
	AudioFrameDesc _frameDesc;
};

}

#endif