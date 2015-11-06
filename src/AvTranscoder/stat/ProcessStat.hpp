#ifndef  _AV_TRANSCODER_PROCESSSTAT_HPP
#define  _AV_TRANSCODER_PROCESSSTAT_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/stat/VideoStat.hpp>
#include <AvTranscoder/stat/AudioStat.hpp>

#include <map>

namespace avtranscoder
{

/**
 * @brief ProcessStat contains statistics given after the process.
 * @see Transcoder::process methods
 */
class AvExport ProcessStat
{
public:
	ProcessStat()
	: _videoStats()
	{}

	void addVideoStat( const size_t streamIndex, const VideoStat& videoStat );
	void addAudioStat( const size_t streamIndex, const AudioStat& audioStat );

	VideoStat& getVideoStat( const size_t streamIndex ) { return _videoStats.at(streamIndex); }
	AudioStat& getAudioStat( const size_t streamIndex ) { return _audioStats.at(streamIndex); }

private:
	std::map<size_t, VideoStat> _videoStats;  ///< Key: streamIndex, Value: statistic video results
	std::map<size_t, AudioStat> _audioStats;  ///< Key: streamIndex, Value: statistic audio results
};

}

#endif
