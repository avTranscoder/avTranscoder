#ifndef _AVPLAYER_AVREADER_
#define _AVPLAYER_AVREADER_


#include <AvTranscoder/InputStream.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/InputStreamVideo.hpp>
#include <AvTranscoder/InputFile.hpp>

#include <AvTranscoder/ColorTransform.hpp>

#include <AvTranscoder/Metadatas/Print.hpp>

#include "Reader.hpp"

class AvReader : public Reader
{
public:
	AvReader( std::string filename )
		: m_inputFile( filename )
		, m_inputStreamVideo( NULL )
		, m_sourceImage( NULL )
		, m_imageToDisplay( NULL )
	{
		m_inputFile.analyse();
		m_videoStream = m_inputFile.getProperties().videoStreams.at(0).streamId;

		m_inputFile.readStream( m_videoStream );

		m_inputStreamVideo = new avtranscoder::InputStreamVideo( m_inputFile.getStream( m_videoStream ) );
		
		m_sourceImage = new avtranscoder::Image( m_inputFile.getStream( m_videoStream ).getVideoDesc().getImageDesc() );

		pixel.setBitsPerPixel( getComponents() * getBitDepth() );
		pixel.setComponents( getComponents() );
		pixel.setColorComponents( avtranscoder::eComponentRgb );
		pixel.setSubsampling( avtranscoder::eSubsamplingNone );
		pixel.setAlpha( false );
		pixel.setPlanar( false );

		imageDescToDisplay.setWidth ( m_sourceImage->desc().getWidth() );
		imageDescToDisplay.setHeight( m_sourceImage->desc().getHeight() );
		imageDescToDisplay.setDar   ( m_sourceImage->desc().getDar() );

		imageDescToDisplay.setPixel ( pixel.findPixel() );
		
		m_imageToDisplay = new avtranscoder::Image( imageDescToDisplay );
	}

	~AvReader()
	{
		delete m_inputStreamVideo;
		delete m_sourceImage;
		delete m_imageToDisplay;
	}

	size_t getWidth()
	{
		return m_inputFile.getProperties().videoStreams.at(0).width;
	};

	size_t getHeight()
	{
		return m_inputFile.getProperties().videoStreams.at(0).height;
	}

	size_t getComponents()
	{
		return m_inputFile.getProperties().videoStreams.at(0).componentsCount;
	}

	size_t getBitDepth()
	{
		return 8;
	}

	const char* readNextFrame()
	{
		++currentFrame;
		m_inputStreamVideo->readNextFrame( *m_sourceImage );
		m_colorTransform.convert( *m_sourceImage, *m_imageToDisplay );
		return (const char*)m_imageToDisplay->getPtr();
	}

	const char* readPrevFrame()
	{
		--currentFrame;
		return readFrameAt( currentFrame );
	}

	const char* readFrameAt( const size_t frame )
	{
		// /std::cout << "seek at " << frame << std::endl;
		m_inputFile.seekAtFrame( frame );
		m_inputStreamVideo->flushDecoder();
		m_inputStreamVideo->readNextFrame( *m_sourceImage );
		m_colorTransform.convert( *m_sourceImage, *m_imageToDisplay );
		return (const char*)m_imageToDisplay->getPtr();
	}

	void printMetadatas()
	{
		displayMetadatas( m_inputFile );
	}

private:
	avtranscoder::InputFile m_inputFile;
	avtranscoder::InputStreamVideo* m_inputStreamVideo;

	avtranscoder::Image* m_sourceImage;
	avtranscoder::Image* m_imageToDisplay;

	avtranscoder::Pixel pixel;
	avtranscoder::ImageDesc imageDescToDisplay;

	avtranscoder::ColorTransform m_colorTransform;
	size_t m_videoStream;
};

#endif