#ifndef _AVPLAYER_AVREADER_
#define _AVPLAYER_AVREADER_

#include <mediaCore/progress/ConsoleProgress.hpp>

#include <mediaIO/file/InputFile.hpp>
#include <mediaIO/essenceStream/AvInputAudio.hpp>
#include <mediaIO/essenceStream/AvInputVideo.hpp>
#include <mediaIO/mediaProperty/printMediaProperty.hpp>

#include <mediaTransform/VideoTransform.hpp>

#include "Reader.hpp"

using namespace mediaengine;

class MediaReader : public Reader
{
public:
	MediaReader( const std::string& filename )
		: _inputFile( filename )
		, _inputVideo( NULL )
		, _sourceImage( NULL )
		, _imageToDisplay( NULL )
	{
		mediacore::ConsoleProgress p;

		_inputFile.analyse( p );
		_videoStream = _inputFile.getProperties().videoStreams.at(0).streamId;

		_inputFile.readStream( _videoStream );

		_inputVideo = new mediaio::AvInputVideo( _inputFile.getStream( _videoStream ) );
		
		_inputVideo->setup();

		_sourceImage = new mediacore::VideoFrame( _inputFile.getStream( _videoStream ).getVideoCodec().getVideoFrameDesc() );

		_pixel.setBitsPerPixel( getComponents() * getBitDepth() );
		_pixel.setComponents( getComponents() );
		_pixel.setColorComponents( mediacore::eComponentRgb );
		_pixel.setSubsampling( mediacore::eSubsamplingNone );
		_pixel.setAlpha( false );
		_pixel.setPlanar( false );

		_videoFrameDescToDisplay.setWidth( _sourceImage->desc().getWidth() );
		_videoFrameDescToDisplay.setHeight( _sourceImage->desc().getHeight() );
		_videoFrameDescToDisplay.setDar( _sourceImage->desc().getDar() );
		
		_videoFrameDescToDisplay.setPixel( _pixel.findPixel() );
		
		_imageToDisplay = new mediacore::VideoFrame( _videoFrameDescToDisplay );
	}

	~MediaReader()
	{
		delete _inputVideo;
		delete _sourceImage;
		delete _imageToDisplay;
	}

	size_t getWidth()
	{
		return _inputFile.getProperties().videoStreams.at(0).width;
	};

	size_t getHeight()
	{
		return _inputFile.getProperties().videoStreams.at(0).height;
	}

	size_t getComponents()
	{
		return _inputFile.getProperties().videoStreams.at(0).componentsCount;
	}

	size_t getBitDepth()
	{
		return 8;
	}

	const char* readNextFrame()
	{
		++_currentFrame;
		_inputVideo->readNextFrame( *_sourceImage );
		_videoTransform.convert( *_sourceImage, *_imageToDisplay );
		return (const char*)_imageToDisplay->getPtr();
	}

	const char* readPrevFrame()
	{
		--_currentFrame;
		return readFrameAt( _currentFrame );
	}

	const char* readFrameAt( const size_t frame )
	{
		// /std::cout << "seek at " << frame << std::endl;
		_inputFile.seekAtFrame( frame );
		_inputVideo->flushDecoder();
		return readNextFrame();
	}

	void printMetadatas()
	{
		std::cout << _inputFile << std::endl;
	}

private:
	mediaio::InputFile   _inputFile;

	mediaio::AvInputVideo* _inputVideo;

	mediacore::VideoFrame* _sourceImage;
	mediacore::VideoFrame* _imageToDisplay;

	mediacore::Pixel          _pixel;
	mediacore::VideoFrameDesc _videoFrameDescToDisplay;

	mediatransform::VideoTransform _videoTransform;
	size_t _videoStream;
};

#endif
