#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/File/OutputFile.hpp>
#include <AvTranscoder/CodedStream/InputStream.hpp>
#include <AvTranscoder/ProgressListener.hpp>
#include <AvTranscoder/DummyInputStream.hpp>
#include <AvTranscoder/Profile.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

class StreamTranscoder;

class Transcoder
{

public:
	struct InputStreamDesc {
		size_t streamId;
		std::string filename;
		Profile::ProfileDesc transcodeProfile;

		InputStreamDesc( const size_t& sId, const std::string& filename, const Profile::ProfileDesc& profile )
			: streamId( sId )
			, filename( filename )
			, transcodeProfile( profile )
		{
		}
		
		InputStreamDesc( const size_t& sId, const std::string& filename, const std::string& profileName )
			: streamId( sId )
			, filename( filename )
		{
			try
			{
				Profile p( true );
				transcodeProfile = p.getProfile( profileName );
			}
			// if the profile doesn't exist
			catch( std::exception& e )
			{
				Profile::ProfileDesc emptyDesc;
				emptyDesc[ Profile::avProfileIdentificator ] = "";
				emptyDesc[ Profile::avProfileIdentificatorHuman ] = "";
				
				transcodeProfile = emptyDesc;
			}
		}
	};

	typedef std::vector< InputStreamDesc > InputStreamsDesc;

	Transcoder( OutputFile& outputFile );
	~Transcoder();

	/**
	 * @brief Add a stream and set a profile
	 * @note If profile is empty, add a dummy stream.
	 */
	void add( const std::string& filename, const size_t streamIndex, const std::string& profileName = "" );
	/**
	 * @brief Add a srteam and set a custom profile
	 * @note Profile will be updated, be sure to pass unique profile name.
	 */
	void add( const std::string& filename, const size_t streamIndex, const Profile::ProfileDesc& profileDesc );
	/**
	 * @brief Add a list of streams.
	 */
	void add( const InputStreamsDesc& streamDefs );

	bool processFrame();

	void process( ProgressListener& progress );

private:
	void add( const InputStreamDesc& streamDefinition );
	
	bool getStreamsNextPacket( std::vector< DataStream >& dataStreams );

private:
	OutputFile&                      _outputFile;
	std::vector< InputFile* >        _inputFiles;

	std::vector< InputStream* >      _inputStreams;
	std::vector< StreamTranscoder* > _streamTranscoders;
	
	std::vector< DummyInputStream* > _dummyInputStreams;
	
	Profile _profile;
};

}

#endif