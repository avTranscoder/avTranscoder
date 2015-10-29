#ifndef _AV_TRANSCODER_STREAM_INPUT_STREAM_HPP_
#define _AV_TRANSCODER_STREAM_INPUT_STREAM_HPP_

#include "IInputStream.hpp"

#include <queue>

struct AVStream;

namespace avtranscoder
{

class InputFile;

class AvExport InputStream : public IInputStream
{
private:
	InputStream( const InputStream& inputStream );
	InputStream& operator=( const InputStream& inputStream );

public:
	InputStream( InputFile& inputFile, const size_t streamIndex );
	~InputStream( );

	bool readNextPacket( CodedData& data );

	const StreamProperties& getProperties() const;
	size_t getStreamIndex() const { return _streamIndex; }

	VideoCodec& getVideoCodec();
	AudioCodec& getAudioCodec();
	DataCodec& getDataCodec();

	//@{
	/**
	 * @brief Functions about buffering
	 * @see IInputStream methods
	 */
	void activate( const bool activate = true ){ _isActivated = activate; };
	bool isActivated() const { return _isActivated; };
	void addPacket( const AVPacket& packet );
	void clearBuffering();
	//@}

private:
	InputFile* _inputFile;  ///< Has link (no ownership)
	ICodec* _codec;  ///< Has ownership

	std::queue<CodedData> _streamCache;  ///< Cache of packet data already read and corresponding to this stream

	size_t _streamIndex;  ///<  Index of the stream in the input file
	bool _isActivated;  ///< If the stream is activated, data read from it will be buffered
};

}

#endif