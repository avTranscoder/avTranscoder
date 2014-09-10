#ifndef _AV_TRANSCODER_CODED_STREAM_AV_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_AV_INPUT_STREAM_HPP_

#include "InputStream.hpp"

#include <AvTranscoder/CodedStructures/DataStream.hpp>

namespace avtranscoder
{

class InputFile;

class AvExport AvInputStream : public InputStream
{
public:
	AvInputStream( );

	AvInputStream( InputFile& inputFile, const size_t streamIndex );
	~AvInputStream( );

	AvInputStream( const AvInputStream& inputStream )
		: InputStream( )
		, _inputFile( inputStream._inputFile )
		, _streamIndex( inputStream._streamIndex )
		, _bufferized( inputStream._bufferized )
	{
	}

	size_t getStreamIndex() const { return _streamIndex; }

	bool readNextPacket( DataStream& data );

	// Stream properties
	VideoDesc getVideoDesc() const;
	AudioDesc getAudioDesc() const;
	DataDesc  getDataDesc()  const;

	AVMediaType getStreamType() const;

	double getDuration() const;
	double getPacketDuration() const;

	void addPacket( AVPacket& packet );

	void setBufferred( const bool bufferized ){ _bufferized = bufferized; };
	bool getBufferred() const { return _bufferized; };

	void clearBuffering();

private:
	InputFile*       _inputFile;
	std::vector<DataStream> _streamCache;

	VideoDesc        _videoDesc;
	AudioDesc        _audioDesc;

	int              _packetDuration;
	size_t           _streamIndex;
	bool             _bufferized;
};

}

#endif