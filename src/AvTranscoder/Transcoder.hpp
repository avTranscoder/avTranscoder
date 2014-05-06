#ifndef _AV_TRANSCODER_OUTPUT_STREAM_VIDEO_HPP_
#define _AV_TRANSCODER_OUTPUT_STREAM_VIDEO_HPP_

#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/OutputFile.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

class Transcoder
{
public:
	typedef std::vector< std::pair< std::string, size_t > > StreamsDefinition;

	Transcoder( const std::string& filename );
	Transcoder( OutputFile& outputFile );

	void add( const std::string& filename, const size_t streamIndex );

	void add( const StreamsDefinition& streams );

	void process();

private:
	OutputFile _outputFile;
	std::vector< InputStream > _inputStreams;
};

}

#endif