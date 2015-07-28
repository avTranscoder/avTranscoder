#ifndef _AV_TRANSCODER_STREAM_OUTPUT_STREAM_HPP_
#define _AV_TRANSCODER_STREAM_OUTPUT_STREAM_HPP_

#include "IOutputStream.hpp"

namespace avtranscoder
{

class OutputFile;

class AvExport OutputStream : public IOutputStream
{
public:
	OutputStream( OutputFile& outputFile, const size_t streamIndex );

	size_t getStreamIndex() const { return _streamIndex; }
	float getStreamDuration() const;
	size_t getNbFrames() const;  ///< If audio stream, returns number of packets

	IOutputStream::EWrappingStatus wrap( const CodedData& data );

private:
	OutputFile* _outputFile;  ///< Has link (no ownership)

	size_t _streamIndex;  ///<  Index of the stream in the output file
};

}

#endif
