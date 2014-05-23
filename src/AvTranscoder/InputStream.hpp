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

#include "InputFile.hpp"

namespace avtranscoder
{

class AvExport InputStream
{
public:
	InputStream( const InputFile* inputFile, const size_t streamIndex );
	~InputStream( );

	InputStream( const InputStream& inputStream )
		: m_inputFile( inputStream.m_inputFile )
		, m_streamIndex( inputStream.m_streamIndex )
	{
	}

	size_t getStreamIndex() const { return m_streamIndex; }

	bool readNextPacket( DataStream& data );

	// Stream propeerties
	VideoDesc getVideoDesc() const;
	AudioDesc getAudioDesc() const;

	double getDuration() const;
	double getPacketDuration() const;

// protected:
	bool readNextPacket( AVPacket& packet ) const;

private:
	const InputFile* m_inputFile;
	std::vector<DataStream> m_streamCache;

	int              m_packetDuration;
	size_t           m_streamIndex;
};

}

#endif