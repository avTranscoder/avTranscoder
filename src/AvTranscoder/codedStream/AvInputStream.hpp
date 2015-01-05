#ifndef _AV_TRANSCODER_CODED_STREAM_AV_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_CODED_STREAM_AV_INPUT_STREAM_HPP_

#include "IInputStream.hpp"

#include <queue>

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

	bool readNextPacket( CodedData& data );

	size_t getStreamIndex() const { return _streamIndex; }
	double getDuration() const;
	AVMediaType getStreamType() const;

	VideoCodec& getVideoCodec();
	AudioCodec& getAudioCodec();
	DataCodec& getDataCodec();

	//@{
	/**
	 * @brief Functions about buffering
	 */
	void activate( const bool activate = true ){ _isActivated = activate; };
	bool isActivated() const { return _isActivated; };
	void addPacket( AVPacket& packet );
	void clearBuffering();
	//@}

private:
	AVStream* getAVStream() const;

private:
	InputFile* _inputFile;  ///< Has link (no ownership)
	ICodec* _codec;  ///< Has ownership

	std::queue<CodedData> _streamCache;  ///< Cache of packet data already read and corresponding to this stream

	size_t _streamIndex;  ///<  Index of the stream in the input file
	bool _isActivated;  ///< If the stream is activated, data read from it will be buffered
};

}

#endif