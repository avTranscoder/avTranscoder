#ifndef _AV_TRANSCODER_VIDEOREADER_HPP
#define _AV_TRANSCODER_VIDEOREADER_HPP

#include "IReader.hpp"

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>
#include <AvTranscoder/decoder/VideoDecoder.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>

namespace avtranscoder
{

class AvExport VideoReader : public IReader
{
public:
	VideoReader( const std::string& filename, const size_t videoStreamIndex );
	~VideoReader();

	size_t getWidth();
	size_t getHeight();
	size_t getComponents();
	size_t getBitDepth();
	AVPixelFormat getPixelFormat();

	const char* readNextFrame();
	const char* readPrevFrame();
	const char* readFrameAt( const size_t frame );

	void printInfo();

private:
	InputFile _inputFile;
	const VideoProperties* _videoProperties;
	VideoDecoder* _videoDecoder;

	VideoFrame* _sourceImage;
	VideoFrame* _imageToDisplay;

	VideoTransform _videoTransform;

	size_t _videoStreamIndex;
};

}

#endif
