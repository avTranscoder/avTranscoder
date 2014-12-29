#ifndef _AVPLAYER_AVREADER_
#define _AVPLAYER_AVREADER_

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/essenceStream/AvInputAudio.hpp>
#include <AvTranscoder/essenceStream/AvInputVideo.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>
#include <AvTranscoder/mediaProperty/print.hpp>

#include <AvTranscoder/progress/ConsoleProgress.hpp>

#include "Reader.hpp"

class AvReader : public Reader
{
public:
	AvReader( const std::string& filename )
		: _inputFile( filename )
		, _inputVideo( NULL )
		, _sourceImage( NULL )
		, _imageToDisplay( NULL )
	{
		avtranscoder::ConsoleProgress p;

		_inputFile.analyse( p );
		_videoStream = _inputFile.getProperties().getVideoProperties().at(0).getStreamId();

		_inputFile.activateStream( _videoStream );

		_inputVideo = new avtranscoder::AvInputVideo( _inputFile.getStream( _videoStream ) );
		
		_inputVideo->setup();

		_sourceImage = new avtranscoder::VideoFrame( _inputFile.getStream( _videoStream ).getVideoCodec().getVideoFrameDesc() );

		_videoFrameDescToDisplay.setWidth( _sourceImage->desc().getWidth() );
		_videoFrameDescToDisplay.setHeight( _sourceImage->desc().getHeight() );
		_videoFrameDescToDisplay.setDar( _sourceImage->desc().getDar() );
		_videoFrameDescToDisplay.setPixel( getPixelFormat() );
		
		_imageToDisplay = new avtranscoder::VideoFrame( _videoFrameDescToDisplay );
	}

	~AvReader()
	{
		delete _inputVideo;
		delete _sourceImage;
		delete _imageToDisplay;
	}

	size_t getWidth()
	{
		return _inputFile.getProperties().getVideoProperties().at(0).getWidth();
	};

	size_t getHeight()
	{
		return _inputFile.getProperties().getVideoProperties().at(0).getHeight();
	}

	size_t getComponents()
	{
		return _inputFile.getProperties().getVideoProperties().at(0).getPixel().getComponents();
	}

	size_t getBitDepth()
	{
		return _inputFile.getProperties().getVideoProperties().at(0).getPixel().getBitsPerPixel();
	}

	AVPixelFormat getPixelFormat()
	{
		return _inputFile.getProperties().getVideoProperties().at(0).getPixel().getAVPixelFormat();
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
	avtranscoder::InputFile   _inputFile;
	
	avtranscoder::AvInputVideo* _inputVideo;

	avtranscoder::VideoFrame* _sourceImage;
	avtranscoder::VideoFrame* _imageToDisplay;

	avtranscoder::VideoFrameDesc _videoFrameDescToDisplay;

	avtranscoder::VideoTransform _videoTransform;
	size_t _videoStream;
};

#endif
