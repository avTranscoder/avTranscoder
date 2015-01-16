#ifndef _AVPLAYER_AVREADER_
#define _AVPLAYER_AVREADER_

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/decoder/VideoDecoder.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>

#include "Reader.hpp"

class AvReader : public Reader
{
public:
	AvReader( const std::string& filename );
	~AvReader();

	size_t getWidth();
	size_t getHeight();
	size_t getComponents();
	size_t getBitDepth();
	AVPixelFormat getPixelFormat();

	const char* readNextFrame();
	const char* readPrevFrame();
	const char* readFrameAt( const size_t frame );

	void printMetadatas();

private:
	avtranscoder::InputFile   _inputFile;

	avtranscoder::VideoDecoder* _inputVideo;

	avtranscoder::VideoFrame* _sourceImage;
	avtranscoder::VideoFrame* _imageToDisplay;

	avtranscoder::VideoTransform _videoTransform;
	size_t _videoStream;
};

#endif
