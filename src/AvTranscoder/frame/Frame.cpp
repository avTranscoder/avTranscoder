#include "Frame.hpp"

#include <cstring>

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

Frame::Frame( const AVPacket& avPacket )
{
	copyAVPacket( avPacket );
}

Frame::Frame( const Frame& other )
{
	copyAVPacket( other.getAVPacket() );
}

Frame& Frame::operator=( const Frame& other )
{
	copyAVPacket( other.getAVPacket() );
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
		av_grow_packet( &_packet, newSize - _packet.size );
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

void Frame::copyAVPacket( const AVPacket& avPacket )
{
#if AVTRANSCODER_FFMPEG_DEPENDENCY && LIBAVCODEC_VERSION_INT > AV_VERSION_INT(55, 56, 108)
	av_copy_packet( &_packet, &avPacket );
#elif AVTRANSCODER_FFMPEG_DEPENDENCY && LIBAVCODEC_VERSION_INT > AV_VERSION_INT(54, 56, 0)
	av_copy_packet( &_packet, const_cast<AVPacket*>( &avPacket ) );
#else
	// we just care about data, not side properties of AVPacket
	initAVPacket();
	copyData( avPacket.data, avPacket.size );
#endif
}

}
