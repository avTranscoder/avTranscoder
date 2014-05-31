#ifndef _AV_TRANSCODER_INPUT_FILE_HPP_
#define _AV_TRANSCODER_INPUT_FILE_HPP_

#include <AvTranscoder/DatasStructures/DataStreamDesc.hpp>
#include <AvTranscoder/DatasStructures/AudioDesc.hpp>
#include <AvTranscoder/DatasStructures/VideoDesc.hpp>
#include <AvTranscoder/Metadatas/MediaMetadatasStructures.hpp>


#include <string>
#include <vector>

class AVInputFormat;
class AVFormatContext;
class AVStream;
class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class AvInputStream;

class AvExport InputFile
{
public:
	InputFile( const std::string& filename );
	~InputFile();

	std::string getFilename() const { return m_filename; }

	// *** Metadatas section ***
	// run the analyse on the file after a setup.
	InputFile& analyse();
	
	/// get file properties
	const Properties& getProperties() const { return m_properties; }

	AVMediaType getStreamType( size_t index );

	::avtranscoder::AvInputStream& getStream( size_t index );

	AVFormatContext& getFormatContext() const { return *m_formatContext; }

	bool readNextPacket( const size_t streamIndex );

	void seekAtFrame( const size_t frame );

	void readStream( const size_t streamIndex, const bool readStream = true );

protected:
	AVFormatContext*            m_formatContext;
	Properties                  m_properties;
	std::string                 m_filename;
	std::vector<AvInputStream*> m_inputStreams;
};

}

#endif
