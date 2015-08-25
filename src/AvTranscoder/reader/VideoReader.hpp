#ifndef _AV_TRANSCODER_VIDEOREADER_HPP
#define _AV_TRANSCODER_VIDEOREADER_HPP

#include "IReader.hpp"

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>

namespace avtranscoder
{

class AvExport VideoReader : public IReader
{
public:
	VideoReader( const std::string& filename, const size_t videoStreamIndex );
	VideoReader( InputFile& inputFile, const size_t videoStreamIndex );

	~VideoReader();

	size_t getWidth();
	size_t getHeight();
	size_t getComponents();
	size_t getBitDepth();
	AVPixelFormat getPixelFormat();
	const VideoProperties* getVideoProperties() const {return _videoStreamProperties;}

	void printInfo();

private:
	void init();

private:
	const VideoProperties* _videoStreamProperties;  ///< Properties of the video stream read (no ownership, has link)
};

}

#endif
