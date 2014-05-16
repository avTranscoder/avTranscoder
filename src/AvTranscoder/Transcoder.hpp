#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/OutputFile.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

enum EJobStatus
{
	eJobStatusContinue = 0,
	eJobStatusCancel
};

class Transcoder
{
public:
	typedef std::vector< std::pair< std::string, size_t > > StreamsDefinition;

	Transcoder( const std::string& filename );
	Transcoder( OutputFile& outputFile );

	void add( const std::string& filename, const size_t streamIndex );

	void add( const StreamsDefinition& streams );

	void process( EJobStatus (*callback)(double, double) );

private:
	OutputFile _outputFile;
	std::vector< InputStream > _inputStreams;
};

}

#endif