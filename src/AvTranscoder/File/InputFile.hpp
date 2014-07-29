#ifndef _AV_TRANSCODER_FILE_INPUT_FILE_HPP_
#define _AV_TRANSCODER_FILE_INPUT_FILE_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/CodedStructures/DataStream.hpp>
#include <AvTranscoder/CodedStructures/AudioDesc.hpp>
#include <AvTranscoder/CodedStructures/VideoDesc.hpp>

#include <AvTranscoder/CodedStream/AvInputStream.hpp>

#include <AvTranscoder/Metadatas/MediaMetadatasStructures.hpp>
#include <AvTranscoder/ProgressListener.hpp>

#include <AvTranscoder/Profile.hpp>

#include <string>
#include <vector>

class AVFormatContext;

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

	enum EAnalyseLevel
	{
		eAnalyseLevelFast = 0,
		eAnalyseLevelFull = 0,
	};

	/**
	 * @return Return the resource to access
	**/
	std::string getFilename() const { return _filename; }

	/**
	 * @brief Run the analyse on the file after a setup.
	 *        call this function before getProperties().
	 * @param progress callback to get analysis progression
	 **/
	InputFile& analyse( ProgressListener& progress, const EAnalyseLevel level = eAnalyseLevelFull );
	
	/**
	 * @brief Return media properties on the current InputFile.
	 * @note require to launch analyse() before to fill the property struture
	 * @return structure of media metadatas
	 **/
	const Properties& getProperties() const { return _properties; }

	void getProperties( Properties& properties ) const { properties = _properties; }

	/**
	 * @brief Get media file properties using static method.
	 * @param filename input filename
	 * @param progress callback to get analysis progression
	 * @return structure of media metadatas
	 **/
	static Properties analyseFile( const std::string& filename, ProgressListener& progress, const EAnalyseLevel level = eAnalyseLevelFull );

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
	AVFormatContext& getFormatContext() const { return *_formatContext; }

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

	/** 
	 * @brief Indicate that the stream will be bufferized during the read
	 * @param streamIndex specify stream index
	 * @return the reading status of the streamIndex
	 **/
	bool getReadStream( const size_t streamIndex );
	
	/**
	 * @brief Set the format of the input file
     * @param desc: the profile of the input format
     */
	virtual void setProfile( const Profile::ProfileDesc& desc );

protected:
	AVFormatContext*            _formatContext;
	Properties                  _properties;
	std::string                 _filename;
	std::vector<AvInputStream*> _inputStreams;
};

}

#endif
