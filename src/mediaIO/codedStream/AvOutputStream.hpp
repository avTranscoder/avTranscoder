#ifndef _MEDIA_ENGINE_IO_CODED_STREAM_AV_OUTPUT_STREAM_HPP_
#define _MEDIA_ENGINE_IO_CODED_STREAM_AV_OUTPUT_STREAM_HPP_

#include "IOutputStream.hpp"

namespace mediaengine
{
namespace mediaio
{

class OutputFile;

class MediaEngineExport AvOutputStream : public IOutputStream
{
public:
	AvOutputStream( OutputFile& outputFile, const size_t streamIndex );

	size_t getStreamIndex() const { return _streamIndex; }

	bool wrap( const mediacore::CodedData& data );

private:
	OutputFile*      _outputFile;

	size_t           _streamIndex;
};

}
}

#endif
