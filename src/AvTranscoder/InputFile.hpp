#ifndef _AV_TRANSCODER_INPUT_FILE_HPP_
#define _AV_TRANSCODER_INPUT_FILE_HPP_

#include "DatasStructures/DataStreamDesc.hpp"
#include "DatasStructures/VideoDesc.hpp"
#include "Metadatas/MediaMetadatasStructures.hpp"

#include <string>
#include <vector>

class AVInputFormat;
class AVFormatContext;
class AVStream;
class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class InputFile
{
public:
	InputFile();
	~InputFile();

	InputFile& setup( const std::string& file );

	// *** Metadatas section ***
	// run the analyse on the file after a setup.
	InputFile& analyse();
	// get properties on the file
	const Properties& getProperties() const { return properties; }


	VideoDesc getVideoDesc( size_t videoStreamId );
	bool getAudioStream( );


	bool unwrap( DataStream& data, const size_t streamIndex );

protected:
	bool readNextPacket( AVPacket& packet, const size_t streamIndex );

protected:
	AVFormatContext* m_formatContext;
	AVStream*        m_stream;

	Properties       properties;

	std::string      m_filename;
	size_t           m_packetCount;
};

}

#endif
