#ifndef _AVPLAYER_READER_
#define _AVPLAYER_READER_

class Reader
{
public:
	Reader()
		: currentFrame( 0 )
	{}

	virtual size_t getWidth() = 0;
	virtual size_t getHeight() = 0;
	virtual size_t getComponents() = 0;
	virtual size_t getBitDepth() = 0;

	virtual const char* readNextFrame() = 0;
	virtual const char* readPrevFrame() = 0;
	virtual const char* readFrameAt( const size_t frame ) = 0;

	virtual void printMetadatas() = 0;

protected:

	size_t currentFrame;
};

#endif
