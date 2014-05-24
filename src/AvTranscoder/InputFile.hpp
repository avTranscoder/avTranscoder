#ifndef _AV_TRANSCODER_INPUT_FILE_HPP_
#define _AV_TRANSCODER_INPUT_FILE_HPP_

#include "DatasStructures/DataStreamDesc.hpp"
#include "DatasStructures/AudioDesc.hpp"
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

class InputStream;

class AvExport InputFile
{
public:
	InputFile( const std::string& filename );
	~InputFile();

	// *** Metadatas section ***
	// run the analyse on the file after a setup.
	InputFile& analyse();
	/// get file properties
	const Properties& getProperties() const { return m_properties; }

	AVMediaType getStreamType( size_t index );

	::avtranscoder::InputStream& getStream( size_t index );

	AVFormatContext* getFormatContext() const { return m_formatContext; }

	bool readNextPacket( const size_t streamIndex );

	void readStream( const size_t streamIndex, const bool readStream = true );

protected:
	AVFormatContext*         m_formatContext;
	Properties               m_properties;
	std::string              m_filename;
	std::vector<InputStream> m_inputStreams;
};

}

#include "InputStream.hpp"

#endif
