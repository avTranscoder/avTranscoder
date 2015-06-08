#ifndef  _AV_TRANSCODER_VIDEOSTAT_HPP
#define  _AV_TRANSCODER_VIDEOSTAT_HPP

#include <AvTranscoder/common.hpp>

namespace avtranscoder
{

/**
 * @brief Statistics related to a video stream.
 */
class VideoStat
{
public:
	VideoStat( const double duration, const size_t nbFrames )
	: _duration( duration )
	, _nbFrames( nbFrames )
	, _quality( 0 )
	, _psnr( 0 )
	{}

public:
	static double psnr( const double d );

public:
	double _duration;
	size_t _nbFrames;
	size_t _quality;  ///< Between 1 (good) and FF_LAMBDA_MAX (bad). 0 if unknown.
	double _psnr;  ///< 0 if unknown.
};

}

#endif
