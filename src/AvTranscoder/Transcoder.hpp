#ifndef _AV_TRANSCODER_TRANSCODER_HPP_
#define _AV_TRANSCODER_TRANSCODER_HPP_

#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/OutputFile.hpp>
#include <AvTranscoder/ProgressListener.hpp>
#include <AvTranscoder/DummyInputStream.hpp>

#include <string>
#include <vector>

namespace avtranscoder
{

class Transcoder
{
public:
	typedef std::vector< std::pair< std::string, size_t > > StreamsDefinition;

	Transcoder( OutputFile& outputFile );

	~Transcoder();

	void add( const std::string& filename, const size_t streamIndex );

	void add( const StreamsDefinition& streams );

	void process( ProgressListener& progress );

private:
	OutputFile&                      _outputFile;
	std::vector< InputFile* >        _inputFiles;
	std::vector< InputStream* >      _inputStreams;

	std::vector< DummyInputStream* > _dummyInputStreams;
};

}

#include "Transcoder.tcc"

#endif