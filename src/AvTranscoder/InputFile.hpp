#ifndef _AV_TRANSCODER_INPUT_FILE_HPP_
#define _AV_TRANSCODER_INPUT_FILE_HPP_

#include "DatasStructures/DataStreamDesc.hpp"
#include "DatasStructures/AudioDesc.hpp"
#include "DatasStructures/VideoDesc.hpp"
#include "Metadatas/MediaMetadatasStructures.hpp"
#include "InputStream.hpp"


#include <string>
#include <vector>

class AVInputFormat;
class AVFormatContext;
class AVStream;
class AVCodec;
class AVCodecContext;

namespace avtranscoder
{

class AvExport InputFile
{
public:
	InputFile( const std::string& filename );
	~InputFile();

	// *** Metadatas section ***
	// run the analyse on the file after a setup.
	InputFile& analyse();
	// get properties on the file
	const Properties& getProperties() const { return m_properties; }

	InputStream getStream( size_t index );

protected:
	AVFormatContext* m_formatContext;
	Properties       m_properties;
	std::string      m_filename;
};

}

#endif
