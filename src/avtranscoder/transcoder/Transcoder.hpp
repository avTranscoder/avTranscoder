#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <avtranscoder/common.hpp>

#include <avtranscoder/file/InputFile.hpp>
#include <avtranscoder/file/OutputFile.hpp>

#include <avtranscoder/codedStream/IInputStream.hpp>

#include <avtranscoder/essenceStream/GeneratorAudio.hpp>
#include <avtranscoder/essenceStream/GeneratorVideo.hpp>

#include <avtranscoder/progress/IProgress.hpp>

#include <avtranscoder/Profile.hpp>

#include "StreamTranscoder.hpp"

#include <string>
#include <vector>

namespace avtranscoder
{

/**
 * @brief Enum to set a policy of how we manage the transcode in case of several streams.
 * eProcessMethodShortest: stop transcode at the end of the shortest stream.
 * eProcessMethodLongest: stop transcode at the end of the longest stream (default method).
 * eProcessMethodBasedOnStream: stop transcode at the end of an indicated stream (@see _indexBasedStream of Transcoder).
 * eProcessMethodInfinity: stop transcode by outside of avtranscoder.
 */
enum EProcessMethod
{
	eProcessMethodShortest = 0,
	eProcessMethodLongest,
	eProcessMethodBasedOnStream,
	eProcessMethodInfinity,
};

/**
 * @brief A Transcoder manages a list of streams, 
 * and process a transcode to create an output media file.
 */
class AvExport Transcoder
{
public:

	Transcoder( OutputFile& outputFile );
	
	~Transcoder();

	/**
	 * @brief Add a stream and set a profile
	 * @note If profileName is empty, rewrap.
	 */
	void add( const std::string& filename, const size_t streamIndex, const std::string& profileName = "", const size_t offset = 0 );
	/*
	 * @note If filename is empty, add a generated stream.
	 * @note If filename is empty, profileName can't be empty (no sens to rewrap a generated stream).
	 */
	void add( const std::string& filename, const size_t streamIndex, const std::string& profileName, ICodec& codec, const size_t offset = 0 );

	/**
	 * @brief Add a stream and set a custom profile
	 * @note Profile will be updated, be sure to pass unique profile name.
	 */
	void add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc, const size_t offset = 0 );
	/*
	 * @note If filename is empty, add a generated stream.
	 */
	void add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc, ICodec& codec, const size_t offset = 0  );
	
	/**
	 * @brief Add a stream and set a profile
	 * @note If profileName is empty, rewrap.
	 * @note If subStreamIndex is negative, no substream is selected it's the stream. 
	 */
	void add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName = "", const size_t offset = 0 );
	/**
	 * @note If filename is empty, add a generated stream.
	 * @note If filename is empty, profileName can't be empty (no sens to rewrap a generated stream).
	 */
	void add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, ICodec& codec, const size_t offset = 0  );

	/**
	 * @brief Add a stream and set a custom profile
	 * @note Profile will be updated, be sure to pass unique profile name.
	 * @note If subStreamIndex is negative, no substream is selected it's the stream.
	 */
	void add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc, const size_t offset = 0 );
	/**
	 * @note If filename is empty, add a generated stream.
	 */
	void add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc, ICodec& codec, const size_t offset = 0  );

	/**
	 * @brief Add the stream
	 * @note The stream will be deleted in Transcoder's destructor.
	 */
	void add( StreamTranscoder& stream );

	/**
	 * @brief Initialize all streams added, by ensure process necessary frames in case of latency.
	 * @note This can be called several times with no side effects.
	 * @note Can take a little bit of time.
	 */
	void init();
	
	/**
	 * @brief Process the next frame of all streams.
	 * @return if a frame was processed or not.
	 */
	bool processFrame();

	/**
	 * @brief Process all the streams, and ended the process depending on the transcode politic.
	 * @param progress
	 */
	void process( IProgress& progress );

	/**
	 * @brief Set the transcodage politic.
	 * @note Call it after adding the streams.
	 * @note By default eProcessMethodLongest.
	 * @param indexBasedStream: in case of process method eProcessMethodBasedOnStream, stop transcode at the end of the indicated stream.
	 */
	void setProcessMethod( const EProcessMethod eProcessMethod, const size_t indexBasedStream = 0 );

	/**
	 * @brief Set verbose mode for the Transcoder and its streams.
	 * @note If you call it before adding the streams, no verbose mode will be set for the new streams.
	 */
	void setVerbose( bool verbose = true );

	/**
	 * @brief Set FPS of output media file.
	 * @note By default 25 frames per second.
     */
	void setOutputFps( double fps ) { _outputFps = fps; }

private:

	void addRewrapStream( const std::string& filename, const size_t streamIndex );

	void addTranscodeStream( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profile, const size_t offset = 0 );

	void addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, Profile::ProfileDesc& profile, const size_t offset = 0 );

	void addDummyStream( const Profile::ProfileDesc& profile, const ICodec& codec );

	InputFile* addInputFile( const std::string& filename, const size_t streamIndex );

	/**
	 * @brief Get the duration of the stream.
	 */
	double getStreamDuration( size_t indexStream ) const;

	/**
	* @brief Get the duration of the shortest stream.
	* @note if there is only generated streams, return limit of double.
	*/
	double getMinTotalDuration() const;

	/**
	 * @brief Get the duration of the longest stream.
	 * @note if there is only generated streams, return limit of double.
	 */
	double getMaxTotalDuration() const;

private:
	OutputFile&                      _outputFile;  ///< The output media file after process.
	std::vector< InputFile* >        _inputFiles;  ///< The list of input files which contain added streams.

	std::vector< StreamTranscoder* > _streamTranscoders;  ///< The streams of the output media file after process.
	
	std::vector< IInputStream* > _inputStreams;  ///< Objects to manage streams based on existing media files.
	std::vector< GeneratorAudio* > _generatorAudio;  ///< Objects to manage silent audio streams.
	std::vector< GeneratorVideo* > _generatorVideo;  ///< Objects to manage silent video streams (black images).

	Profile _profile;  ///< Objet to get existing profiles, and add new ones for the Transcoder.

	double _outputFps;

	EProcessMethod _eProcessMethod;

	size_t _mainStreamIndex;  ///< Index of stream used to stop the process of transcode in case of eProcessMethodBasedOnStream.

	bool    _verbose;
};

}

#endif
