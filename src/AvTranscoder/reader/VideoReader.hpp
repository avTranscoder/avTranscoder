#ifndef _AV_TRANSCODER_VIDEOREADER_HPP
#define _AV_TRANSCODER_VIDEOREADER_HPP

#include "IReader.hpp"

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/PixelProperties.hpp>
#include <AvTranscoder/mediaProperty/VideoProperties.hpp>

namespace avtranscoder
{

class AvExport VideoReader : public IReader
{
public:
	//@{
	// @param width: if 0, get width of source
	// @param height: if 0, get height of source
	// @param pixelFormat: rgb24 by default (to display)
	//
	VideoReader( const std::string& filename, const size_t videoStreamIndex, const size_t width = 0, const size_t height = 0, const std::string& pixelFormat = "rgb24" );
	VideoReader( InputFile& inputFile, const size_t videoStreamIndex, const size_t width = 0, const size_t height = 0, const std::string& pixelFormat = "rgb24" );
	//@}

	~VideoReader();

	//@{
	// @brief Output info
	size_t getWidth();
	size_t getHeight();
	size_t getComponents();
	size_t getBitDepth();
	AVPixelFormat getPixelFormat();
	//@}

	// @brief Input info
	const VideoProperties* getVideoProperties() const {return _videoStreamProperties;}

	void printInfo();

private:
	void init();

private:
	const VideoProperties* _videoStreamProperties;  ///< Properties of the source video stream read (no ownership, has link)

	//@{
	// @brief Output info
	size_t _width;
	size_t _height;
	PixelProperties _pixelProperties;
	//@}
};

}

#endif
