#ifndef _AVPLAYER_AVREADER_
#define _AVPLAYER_AVREADER_

#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/EssenceStream/InputAudio.hpp>
#include <AvTranscoder/EssenceStream/InputVideo.hpp>
#include <AvTranscoder/EssenceTransform/VideoEssenceTransform.hpp>
#include <AvTranscoder/Metadatas/Print.hpp>

#include "Reader.hpp"

class AvReader : public Reader
{
public:
	AvReader( const std::string& filename )
		: m_inputFile( filename )
		, m_inputVideo( NULL )
		, m_sourceImage( NULL )
		, m_imageToDisplay( NULL )
	{
		avtranscoder::ProgressListener p;

		m_inputFile.analyse( p );
		m_videoStream = m_inputFile.getProperties().videoStreams.at(0).streamId;

		m_inputFile.readStream( m_videoStream );

		m_inputVideo = new avtranscoder::InputVideo( m_inputFile.getStream( m_videoStream ) );
		
		m_inputVideo->setup();

		m_sourceImage = new avtranscoder::Image( m_inputFile.getStream( m_videoStream ).getVideoDesc().getImageDesc() );

		pixel.setBitsPerPixel( getComponents() * getBitDepth() );
		pixel.setComponents( getComponents() );
		pixel.setColorComponents( avtranscoder::eComponentRgb );
		pixel.setSubsampling( avtranscoder::eSubsamplingNone );
		pixel.setAlpha( false );
		pixel.setPlanar( false );

		imageDescToDisplay.setWidth( m_sourceImage->desc().getWidth() );
		imageDescToDisplay.setHeight( m_sourceImage->desc().getHeight() );
		imageDescToDisplay.setDar( m_sourceImage->desc().getDar() );
		
		imageDescToDisplay.setPixel( pixel.findPixel() );
		
		m_imageToDisplay = new avtranscoder::Image( imageDescToDisplay );
	}

	~AvReader()
	{
		delete m_inputVideo;
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
		m_inputVideo->readNextFrame( *m_sourceImage );
		m_videoEssenceTransform.convert( *m_sourceImage, *m_imageToDisplay );
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
		m_inputVideo->flushDecoder();
		return readNextFrame();
	}

	void printMetadatas()
	{
		displayMetadatas( m_inputFile );
	}

private:
	avtranscoder::InputFile m_inputFile;
	
	avtranscoder::InputVideo* m_inputVideo;

	avtranscoder::Image* m_sourceImage;
	avtranscoder::Image* m_imageToDisplay;

	avtranscoder::Pixel pixel;
	avtranscoder::ImageDesc imageDescToDisplay;

	avtranscoder::VideoEssenceTransform m_videoEssenceTransform;
	size_t m_videoStream;
};

#endif
