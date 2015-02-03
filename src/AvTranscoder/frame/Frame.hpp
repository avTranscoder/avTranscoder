#ifndef _AV_TRANSCODER_FRAME_FRAME_HPP_
#define _AV_TRANSCODER_FRAME_FRAME_HPP_

#include <AvTranscoder/common.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

namespace avtranscoder
{

class AvExport Frame
{
public:
	Frame()
	{
		av_init_packet( &_packet );
		_packet.data = NULL;
		_packet.size = 0;
	}

	Frame( const size_t dataSize )
	{
		av_new_packet( &_packet, dataSize );
	}

	Frame(AVPacket& avPacket)
	{
		av_copy_packet( &_packet, &avPacket );
	}

	~Frame()
	{
		av_free_packet( &_packet );
	}

	void resize( const size_t newSize )
	{
		if( (int) newSize < _packet.size )
			av_shrink_packet( &_packet, newSize );
		 else if( (int) newSize > _packet.size )
			av_grow_packet( &_packet, newSize );
	}

	void copyData(unsigned char* buffer, const size_t size)
	{
		_packet.data = buffer;
		_packet.size = size;
	}

	void clear()
	{
		av_free_packet( &_packet );

	        av_init_packet( &_packet );
		_packet.data = NULL;
		_packet.size = 0;
	}

	/// Use this function in pyTest to check if we can modify the buffer
	void assign( const size_t size, const int value )
	{
	    resize( size );
	    memset( _packet.data, value, size );
	}

	AVPacket& getAVPacket() { return _packet; }
	unsigned char* getPtr() { return _packet.data; }
	size_t getSize() const { return _packet.size; }

#ifndef SWIG
	const AVPacket& getAVPacket() const { return _packet; }
	const unsigned char* getPtr() const { return _packet.data; }
#endif

protected:
	AVPacket _packet;
};

// Typedef to represent buffer of coded data.
// Example 1: in case of image, no sense to get size if coded data.
// Example 2: in case of audio, no sense to get number of channels if coded data.
typedef Frame CodedData;

}

#endif
