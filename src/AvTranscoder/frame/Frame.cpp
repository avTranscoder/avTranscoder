#include "Frame.hpp"

namespace avtranscoder
{

Frame::Frame()
{
	initAVPacket();
}

Frame::Frame( const size_t dataSize )
{
	av_new_packet( &_packet, dataSize );
}

Frame::Frame(AVPacket& avPacket)
{
#if LIBAVCODEC_VERSION_MAJOR > 54 || ( LIBAVCODEC_VERSION_MAJOR == 54 && LIBAVCODEC_VERSION_MINOR > 56 )
	av_copy_packet( &_packet, &avPacket );
#else
	// we just care about data, not side properties of AVPacket
	initAVPacket();
	copyData( avPacket.data, avPacket.size );
#endif
}

Frame::~Frame()
{
	av_free_packet( &_packet );
}

void Frame::resize( const size_t newSize )
{
	if( (int) newSize < _packet.size )
		av_shrink_packet( &_packet, newSize );
	 else if( (int) newSize > _packet.size )
		av_grow_packet( &_packet, newSize );
	memset( _packet.data, 0, _packet.size );
}

void Frame::refData( unsigned char* buffer, const size_t size )
{
	_packet.data = buffer;
	_packet.size = size;
}

void Frame::copyData( unsigned char* buffer, const size_t size )
{
	resize( size );
	if ( size != 0 )
		memcpy( _packet.data, buffer, _packet.size );
}

void Frame::refData( Frame& frame )
{
	_packet.data = frame.getData();
	_packet.size = frame.getSize();
}

void Frame::clear()
{
	av_free_packet( &_packet );
	initAVPacket();
}

void Frame::assign( const size_t size, const int value )
{
	resize( size );
	memset( _packet.data, value, size );
}

void Frame::initAVPacket()
{
	av_init_packet( &_packet );
	_packet.data = NULL;
	_packet.size = 0;
}

}
