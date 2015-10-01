#include "OutputStream.hpp"

#include <AvTranscoder/file/OutputFile.hpp>

#include <cassert>

namespace avtranscoder
{

OutputStream::OutputStream( OutputFile& outputFile, const size_t streamIndex )
	: IOutputStream()
	, _outputFile( outputFile )
	, _outputStream( outputFile.getFormatContext().getAVStream( streamIndex ) )
	, _streamIndex( streamIndex )
	, _duration( 0 )
{
}

float OutputStream::getStreamDuration() const
{
	// check floating point exception
	if( _outputStream.time_base.den == 0 || _outputStream.pts.den == 0 )
	{
		LOG_WARN( "Cannot compute stream duration of output stream at index " << _streamIndex )
		return 0.f;
	}

	// if stream PTS is not set, use the duration of all packets wrapped
	if( ! _outputStream.pts.val )
	{
		LOG_WARN( "PTS generation when outputting stream " << _streamIndex << " is not set." )
		if( _duration )
			return av_q2d( _outputStream.codec->time_base ) * _duration;
	}

#if AVTRANSCODER_FFMPEG_DEPENDENCY && LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(55, 40, 100)
	// returns the pts of the last muxed packet, converted from timebase to seconds
	return av_q2d( _outputStream.time_base ) * av_stream_get_end_pts( &_outputStream );
#else
	return av_q2d( _outputStream.time_base ) * ( _outputStream.pts.val + ( _outputStream.pts.num / _outputStream.pts.den ) );
#endif
}

size_t OutputStream::getNbFrames() const
{
	return _outputStream.nb_frames;
}

IOutputStream::EWrappingStatus OutputStream::wrap( const CodedData& data )
{
	// wrap packet
	IOutputStream::EWrappingStatus status = _outputFile.wrap( data, _streamIndex );

	// append duration of the packet to the stream
	_duration += data.getAVPacket().duration;

	return status;
}

}
