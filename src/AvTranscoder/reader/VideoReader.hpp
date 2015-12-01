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
	VideoReader( const std::string& filename, const size_t videoStreamIndex );
	VideoReader( InputFile& inputFile, const size_t videoStreamIndex );
	//@}

	~VideoReader();

	/**
	 * @brief Update width, height and pixelFormat of the output.
	 * @note Will transform the decoded data when read the stream.
	 */
	void updateOutput(const size_t width, const size_t height, const std::string& pixelFormat);

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
