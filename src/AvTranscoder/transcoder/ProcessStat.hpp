#ifndef  _AV_TRANSCODER_PROCESSSTAT_HPP
#define	 _AV_TRANSCODER_PROCESSSTAT_HPP

#include <AvTranscoder/common.hpp>

#include <map>

namespace avtranscoder
{

/**
 * @brief Statistic related to a video stream.
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

/**
 * @brief ProcessStat contains statistic results given after the process.
 * @see Transcoder::process methods
 */
class ProcessStat
{
public:
	ProcessStat()
	: _videoStats()
	{}

	void addVideoStat( const size_t streamIndex, const VideoStat& videoStat );

	VideoStat& getVideoStat( const size_t streamIndex ) { return _videoStats.at(streamIndex); }

private:
	std::map<size_t, VideoStat> _videoStats;  ///< Key: streamIndex, Value: statistic video results
};

}

#endif
