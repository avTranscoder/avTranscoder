#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/OutputFile.hpp>
#include <AvTranscoder/ProgressListener.hpp>
#include <AvTranscoder/DummyInputStream.hpp>

#include <AvTranscoder/StreamTranscoder.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

class Transcoder
{

public:
	struct InputStreamDesc {
		size_t streamId;
		std::string filename;
		std::string transcodeProfile;

		InputStreamDesc( const size_t& sId, const std::string& filename, const std::string& profile )
			: streamId( sId )
			, filename( filename )
			, transcodeProfile( profile )
		{
		}
	};

	typedef std::vector< InputStreamDesc > StreamDefinitions;

	Transcoder( OutputFile& outputFile );
	~Transcoder();

	void add( const std::string& filename, const size_t streamIndex, const std::string& profile );
	void add( const StreamDefinitions& streamDefs );

	void process( ProgressListener& progress );

private:
	OutputFile&                      _outputFile;
	std::vector< InputFile* >        _inputFiles;

	std::vector< InputStream* >      _inputStreams;
	std::vector< StreamTranscoder* > _streamTranscoders;

	std::vector< DummyInputStream* > _dummyInputStreams;
};

}

#include "Transcoder.tcc"

#endif