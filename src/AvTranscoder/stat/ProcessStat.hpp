#ifndef  _AV_TRANSCODER_PROCESSSTAT_HPP
#define  _AV_TRANSCODER_PROCESSSTAT_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/stat/VideoStat.hpp>

#include <map>

namespace avtranscoder
{

/**
 * @brief ProcessStat contains statistics given after the process.
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
