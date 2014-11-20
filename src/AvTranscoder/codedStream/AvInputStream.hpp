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
	AvInputStream( const AvInputStream& inputStream );

	~AvInputStream( );

	size_t getStreamIndex() const { return _streamIndex; }

	bool readNextPacket( CodedData& data );

	// Stream properties
	VideoCodec& getVideoCodec();
	AudioCodec& getAudioCodec();
	DataCodec& getDataCodec();

	AVMediaType getStreamType() const;

	double getDuration() const;

	void addPacket( AVPacket& packet );

	void setBufferred( const bool bufferized ){ _bufferized = bufferized; };
	bool getBufferred() const { return _bufferized; };

	void clearBuffering();

private:
	AVStream* getAVStream() const;

private:
	InputFile* _inputFile;  ///< Has link (no ownership)
	ICodec* _codec;  ///< Has ownership

	std::vector<CodedData> _streamCache;

	size_t _streamIndex;  ///<  Index of the stream in the input file
	bool _bufferized;  ///< If the stream is bufferized
};

}

#endif