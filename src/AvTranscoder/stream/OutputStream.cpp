#include "OutputStream.hpp"

#include <AvTranscoder/file/OutputFile.hpp>

#include <cassert>

namespace avtranscoder
{

OutputStream::OutputStream( OutputFile& outputFile, const size_t streamIndex )
	: IOutputStream()
	, _outputFile( outputFile )
	, _streamIndex( streamIndex )
{
}

float OutputStream::getStreamDuration() const
{
	const AVStream& outputStream = _outputFile->getFormatContext().getAVStream( _streamIndex );

	// check floating point exception
	if( outputStream.time_base.den == 0 || outputStream.pts.den == 0 )
	{
		LOG_WARN( "Cannot compute stream duration of output stream at index " << _streamIndex )
		return 0.f;
	}

#if AVTRANSCODER_FFMPEG_DEPENDENCY && LIBAVFORMAT_VERSION_INT >= AV_VERSION_INT(55, 40, 100)
	// returns the pts of the last muxed packet, converted from timebase to seconds
	return av_q2d( outputStream.time_base ) * av_stream_get_end_pts( &outputStream );
#else
	return av_q2d( outputStream.time_base ) * ( outputStream.pts.val + ( outputStream.pts.num / outputStream.pts.den ) );
#endif
}

size_t OutputStream::getNbFrames() const
{
	const AVStream& outputStream = _outputFile->getFormatContext().getAVStream( _streamIndex );
	return outputStream.nb_frames;
}

IOutputStream::EWrappingStatus OutputStream::wrap( const CodedData& data )
{
	// wrap packet
	return _outputFile.wrap( data, _streamIndex );
}

}
