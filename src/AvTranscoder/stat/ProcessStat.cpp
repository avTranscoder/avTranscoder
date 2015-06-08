#include "ProcessStat.hpp"

#include <utility>

namespace avtranscoder
{

void ProcessStat::addVideoStat( const size_t streamIndex, const VideoStat& videoStat )
{
	_videoStats.insert( std::make_pair( streamIndex, videoStat ) );
}

void ProcessStat::addAudioStat( const size_t streamIndex, const AudioStat& audioStat )
{
	_audioStats.insert( std::make_pair( streamIndex, audioStat ) );
}

}
