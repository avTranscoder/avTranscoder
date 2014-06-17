#ifndef _AV_TRANSCODER_AV_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_AV_INPUT_STREAM_HPP_

#include <AvTranscoder/InputStream.hpp>

namespace avtranscoder
{

class InputFile;

class AvExport AvInputStream : public InputStream
{
public:
	AvInputStream( );

	AvInputStream( InputFile& inputFile, const size_t streamIndex );
	~AvInputStream( );

	AvInputStream( const AvInputStream& inputStream )
		: InputStream( )
		, m_inputFile( inputStream.m_inputFile )
		, m_streamIndex( inputStream.m_streamIndex )
		, m_bufferized( inputStream.m_bufferized )
	{
	}

	size_t getStreamIndex() const { return m_streamIndex; }

	bool readNextPacket( DataStream& data );

	// Stream propeerties
	VideoDesc getVideoDesc() const;
	AudioDesc getAudioDesc() const;

	AVMediaType getStreamType() const;

	double getDuration() const;
	double getPacketDuration() const;

	void addPacket( AVPacket& packet );

	void setBufferred( const bool bufferized ){ m_bufferized = bufferized; };

	void clearBuffering();

private:
	InputFile*       m_inputFile;
	std::vector<DataStream> m_streamCache;

	VideoDesc        m_videoDesc;
	AudioDesc        m_audioDesc;

	int              m_packetDuration;
	size_t           m_streamIndex;
	bool             m_bufferized;
};

}

#endif