#ifndef _AV_TRANSCODER_CODED_STREAM_AV_OUTPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_AV_OUTPUT_STREAM_HPP_

#include "IOutputStream.hpp"

namespace avtranscoder
{

class OutputFile;

class AvExport AvOutputStream : public IOutputStream
{
public:
	AvOutputStream( OutputFile& outputFile, const size_t streamIndex );
	~AvOutputStream( );

	AvOutputStream( const AvOutputStream& outputStream )
		: IOutputStream( )
		, _outputFile( outputStream._outputFile )
		, _streamIndex( outputStream._streamIndex )
	{
	}

	size_t getStreamIndex() const { return _streamIndex; }

	bool wrap( CodedData& data );

	// Stream propeerties
	VideoCodec getVideoCodec() const;
	AudioCodec getAudioCodec() const;

private:
	OutputFile*      _outputFile;

	VideoCodec        _videoDesc;
	AudioCodec        _audioDesc;

	size_t           _streamIndex;
};

}

#endif
