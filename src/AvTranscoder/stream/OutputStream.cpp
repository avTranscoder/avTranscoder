#include "OutputStream.hpp"

#include <AvTranscoder/file/OutputFile.hpp>

#include <cassert>

namespace avtranscoder
{

OutputStream::OutputStream( OutputFile& outputFile, const size_t streamIndex )
	: IOutputStream()
	, _outputFile( outputFile )
	, _outputAVStream( outputFile.getFormatContext().getAVStream( streamIndex ) )
	, _streamIndex( streamIndex )
	, _wrappedPacketsDuration( 0 )
	, _lastWrappedPacketDuration( 0 )
	, _isPTSGenerated( false )
{
}

float OutputStream::getStreamDuration() const
{
	const AVFrac& outputPTS = _outputAVStream.pts;
	const AVRational& outputTimeBase = _outputAVStream.time_base;

	// check floating point exception
	if( outputTimeBase.den == 0 || outputPTS.den == 0 )
	{
		LOG_WARN( "Cannot compute stream duration of output stream at index " << _streamIndex )
		return 0.f;
	}

	if( _wrappedPacketsDuration )
		return av_q2d( outputTimeBase ) * _wrappedPacketsDuration;
#if AVTRANSCODER_FFMPEG_DEPENDENCY && LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(55, 40, 100)
	// returns the pts of the last muxed packet, converted from timebase to seconds
	return av_q2d( outputTimeBase ) * av_stream_get_end_pts( &_outputAVStream );
#else
	return av_q2d( outputTimeBase ) * ( outputPTS.val + ( outputPTS.num / outputPTS.den ) );
#endif
}

size_t OutputStream::getNbFrames() const
{
	return _outputAVStream.nb_frames;
}

IOutputStream::EWrappingStatus OutputStream::wrap( const CodedData& data )
{
	// If stream PTS will be generated at rewrap time
	if( ! _isPTSGenerated && (data.getAVPacket().pts == 0 || data.getAVPacket().pts == AV_NOPTS_VALUE) && data.getAVPacket().dts == AV_NOPTS_VALUE )
	{
		LOG_WARN( "PTS of output stream " << _streamIndex << " is generated at rewrap time." )
		_isPTSGenerated = true;
	}

	// wrap packet
	IOutputStream::EWrappingStatus status = _outputFile.wrap( data, _streamIndex );

	// Store duration of the last packet wrapped
	if( data.getAVPacket().duration != 0  && data.getAVPacket().duration != _lastWrappedPacketDuration )
		_lastWrappedPacketDuration = data.getAVPacket().duration;

	// Append duration of the packet to the stream
	if( data.getAVPacket().duration )
		_wrappedPacketsDuration += data.getAVPacket().duration;
	else
		_wrappedPacketsDuration += _lastWrappedPacketDuration;

	return status;
}

}
