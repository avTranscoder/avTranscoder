#ifndef _AV_TRANSCODER_DUMMY_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_DUMMY_INPUT_STREAM_HPP_

#include <AvTranscoder/InputStream.hpp>

#include <AvTranscoder/InputStream.hpp>
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

	void setImageDesc( ImageDesc& imageDesc ){ m_imageDesc = imageDesc; }
	void setFrameDesc( AudioFrameDesc& frameDesc ){ m_frameDesc = frameDesc; }

	VideoDesc getVideoDesc() const;
	AudioDesc getAudioDesc() const;

	AVMediaType getStreamType() const;

	double getDuration() const;
	double getPacketDuration() const;

	void setBufferred( const bool bufferized ){ }

	void clearBuffering();

private:
	VideoDesc m_videoDesc;
	AudioDesc m_audioDesc;

	ImageDesc      m_imageDesc;
	AudioFrameDesc m_frameDesc;
};

}

#endif