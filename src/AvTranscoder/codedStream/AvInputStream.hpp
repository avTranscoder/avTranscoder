#ifndef _AV_TRANSCODER_CODED_STREAM_AV_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_AV_INPUT_STREAM_HPP_

#include "IInputStream.hpp"

struct AVStream;

namespace avtranscoder
{

class InputFile;

class AvExport AvInputStream : public IInputStream
{
public:
	AvInputStream( InputFile& inputFile, const size_t streamIndex );
	~AvInputStream( );

	AvInputStream( const AvInputStream& inputStream )
		: IInputStream( )
		, _inputFile( inputStream._inputFile )
		, _codec( inputStream._codec )
		, _streamIndex( inputStream._streamIndex )
		, _bufferized( inputStream._bufferized )
	{
	}

	size_t getStreamIndex() const { return _streamIndex; }

	bool readNextPacket( CodedData& data );

	// Stream properties
	VideoCodec& getVideoCodec();
	AudioCodec& getAudioCodec();
	DataCodec& getDataCodec();

	AVMediaType getStreamType() const;

	double getDuration() const;
	double getPacketDuration() const;

	void addPacket( AVPacket& packet );

	void setBufferred( const bool bufferized ){ _bufferized = bufferized; };
	bool getBufferred() const { return _bufferized; };

	void clearBuffering();

private:
	AVStream* getAVStream() const;

private:
	InputFile*       _inputFile;
	std::vector<CodedData> _streamCache;

	ICodec* _codec;  ///< Has ownership

	int              _packetDuration;
	size_t           _streamIndex;
	bool             _bufferized;
};

}

#endif