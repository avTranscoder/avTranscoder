#ifndef _AV_TRANSCODER_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_HPP_

#include "DatasStructures/DataStreamDesc.hpp"
#include "DatasStructures/AudioDesc.hpp"
#include "DatasStructures/VideoDesc.hpp"

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
    #define __STDC_CONSTANT_MACROS
#endif
#include <libavformat/avformat.h>
}

#include <string>
#include <iostream>

namespace avtranscoder
{

class AvExport InputStream
{
public:
	InputStream( const std::string& filename, const size_t streamIndex );
	~InputStream( );

	InputStream( const InputStream& inputStream )
		: m_formatContext( NULL )
		, m_streamIndex( inputStream.m_streamIndex )
	{
		init( inputStream.m_formatContext->filename );
	}

	size_t getStreamIndex() const { return m_streamIndex; }

	bool readNextPacket( DataStream& data );

	// Stream propeerties
	VideoDesc getVideoDesc() const;
	AudioDesc getAudioDesc() const;

	double getDuration() const;
	double getPacketDuration() const;

	AVCodecID getVideoCodecId() const { return m_formatContext->streams[m_streamIndex]->codec->codec_id; }
	
// protected:
	bool readNextPacket( AVPacket& packet ) const;


private:
	void init( const std::string& filename );

private:
	AVFormatContext* m_formatContext;
	int              m_packetDuration;
	size_t           m_streamIndex;
};

}

#endif