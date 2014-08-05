#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/File/OutputFile.hpp>

#include <AvTranscoder/CodedStream/InputStream.hpp>

#include <AvTranscoder/EssenceStream/DummyAudio.hpp>
#include <AvTranscoder/EssenceStream/DummyVideo.hpp>

#include <AvTranscoder/ProgressListener.hpp>

#include <AvTranscoder/Profile.hpp>

#include "StreamTranscoder.hpp"

#include <string>
#include <vector>

namespace avtranscoder
{

/**
 * @brief: Enum to set a policy of how we manage the transcode in case of several streams.
 * eProcessMethodShortest: stop transcode at the end of the shortest stream.
 * eProcessMethodLongest: stop transcode at the end of the longest stream (default method).
 * eProcessMethodInfinity: stop transcode by outside of avTranscoder.
 */
enum EProcessMethod
{
	eProcessMethodShortest = 0,
	eProcessMethodLongest,
	eProcessMethodInfinity,
};

class Transcoder
{
public:

	Transcoder( OutputFile& outputFile );
	
	~Transcoder();

	/**
	 * @brief Add a stream and set a profile
	 * @note If profileName is empty, rewrap.
	 */
	void add( const std::string& filename, const size_t streamIndex, const std::string& profileName = "" );
	/*
	 * @note If filename is empty, add a dummy stream.
	 * @note If filename is empty, profileName can't be empty (no sens to rewrap a dummy stream).
	 */
	void add( const std::string& filename, const size_t streamIndex, const std::string& profileName, CodedDesc& essenceDesc );

	/**
	 * @brief Add a stream and set a custom profile
	 * @note Profile will be updated, be sure to pass unique profile name.
	 */
	void add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc );
	/*
	 * @note If filename is empty, add a dummy stream.
	 */
	void add( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profileDesc, CodedDesc& essenceDesc );
	
	/**
	 * @brief Add a stream and set a profile
	 * @note If profileName is empty, rewrap.
	 * @note If subStreamIndex is negative, no substream is selected it's the stream. 
	 */
	void add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName = "" );
	/**
	 * @note If filename is empty, add a dummy stream.
	 * @note If filename is empty, profileName can't be empty (no sens to rewrap a dummy stream).
	 */
	void add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, const std::string& profileName, CodedDesc& essenceDesc );

	/**
	 * @brief Add a stream and set a custom profile
	 * @note Profile will be updated, be sure to pass unique profile name.
	 * @note If subStreamIndex is negative, no substream is selected it's the stream.
	 */
	void add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc );
	/**
	 * @note If filename is empty, add a dummy stream.
	 */
	void add( const std::string& filename, const size_t streamIndex, const int subStreamIndex, Profile::ProfileDesc& profileDesc, CodedDesc& essenceDesc );

	/**
	 * @brief Add the stream
	 * @note The stream will be deleted in Transcoder's destructor.
	 */
	void add( StreamTranscoder& stream );
	
	bool processFrame();

	void process( ProgressListener& progress );

	void setProcessMethod( const EProcessMethod eProcessMethod );

	void setVerbose( bool verbose = true );

private:

	void addRewrapStream( const std::string& filename, const size_t streamIndex );

	void addTranscodeStream( const std::string& filename, const size_t streamIndex, Profile::ProfileDesc& profile );

	void addTranscodeStream( const std::string& filename, const size_t streamIndex, const size_t subStreamIndex, Profile::ProfileDesc& profile );

	void addDummyStream( const Profile::ProfileDesc& profile, const CodedDesc& essenceDesc );

	InputFile* addInputFile( const std::string& filename, const size_t streamIndex );

private:
	OutputFile&                      _outputFile;
	std::vector< InputFile* >        _inputFiles;

	std::vector< InputStream* >      _inputStreams;
	std::vector< StreamTranscoder* > _streamTranscoders;
	
	std::vector< DummyAudio* > _dummyAudio;
	std::vector< DummyVideo* > _dummyVideo;

	Profile _profile;

	size_t _finalisedStreams;
	EProcessMethod _eProcessMethod;

	bool    _verbose;
};

}

#endif
