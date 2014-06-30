#ifndef _AV_TRANSCODER_CODED_STREAM_AV_OUTPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_AV_OUTPUT_STREAM_HPP_

#include "OutputStream.hpp"

namespace avtranscoder
{

class OutputFile;

class AvExport AvOutputStream : public OutputStream
{
public:
	AvOutputStream( );

	AvOutputStream( OutputFile& outputFile, const size_t streamIndex );
	~AvOutputStream( );

	AvOutputStream( const AvOutputStream& outputStream )
		: OutputStream( )
		, _outputFile( outputStream._outputFile )
		, _streamIndex( outputStream._streamIndex )
	{
	}

	size_t getStreamIndex() const { return _streamIndex; }

	bool wrap( DataStream& data );

	// Stream propeerties
	VideoDesc getVideoDesc() const;
	AudioDesc getAudioDesc() const;

private:
	OutputFile*      _outputFile;

	VideoDesc        _videoDesc;
	AudioDesc        _audioDesc;

	size_t           _streamIndex;
};

}

#endif
