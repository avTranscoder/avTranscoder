#ifndef _AV_TRANSCODER_INPUT_FILE_HPP_
#define _AV_TRANSCODER_INPUT_FILE_HPP_

#include <AvTranscoder/AvInputStream.hpp>
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

class AvExport InputFile
{
public:
	/**
	 * @brief Open a media file
	 * @param filename resource to access
	 * \exception runtime_error launched if unable to open file
	**/
	InputFile( const std::string& filename );

	~InputFile();

	/**
	 * @return Return the resource to access
	**/
	std::string getFilename() const { return m_filename; }

	/**
	 * @brief Run the analyse on the file after a setup.
	 *        call this function before getProperties().
	 **/
	InputFile& analyse();
	
	/**
	 * @brief Return media properties on the current InputFile.
	 * @note require to launch analyse() before to fill the property struture
	 * @return structure of media metadatas
	 **/
	const Properties& getProperties() const { return m_properties; }

	void getProperties( Properties& properties ) const { properties = m_properties; }

	/**
	 * @brief Get media file properties using static method.
	 * @return structure of media metadatas
	 **/
	static Properties analyseFile( const std::string& filename );

	/**
	 * @brief Get stream type: video, audio, subtitle, etc.
	 * @param index stream index
	 * @return media stream type of specified index stream
	 **/
	AVMediaType getStreamType( size_t index );

	/**
	 * @brief Get stream type: video, audio, subtitle, etc.
	 * @param index stream index
	 * @return media stream type of specified index stream
	 **/
	AvInputStream& getStream( size_t index );

	/**
	 * @brief Get LibAV/FFmpeg AVFormatContext
	 * @return format context on current InputFile
	 **/
	AVFormatContext& getFormatContext() const { return *m_formatContext; }

	/**
	 * @brief Read the next packet for the specified stream
	 * @note For performances, each readed stream needs to be bufferized using the readStream() method.
	 * @return if next packet was succefully readed
	 **/
	bool readNextPacket( const size_t streamIndex );

	/**
	 * @brief Seek input stream at specified frame
	 * @note clean also buffers in each InputStream
	 * @return if next packet was succefully readed
	 **/
	void seekAtFrame( const size_t frame );

	/** 
	 * @brief Indicate that the stream should be bufferized
	 * @param streamIndex index of the stream need to be read 
	 * @param readStream specify if the stream need to be buffurized 
	 **/
	void readStream( const size_t streamIndex, const bool readStream = true );

protected:
	AVFormatContext*            m_formatContext;
	Properties                  m_properties;
	std::string                 m_filename;
	std::vector<AvInputStream*> m_inputStreams;
};

}

#endif
