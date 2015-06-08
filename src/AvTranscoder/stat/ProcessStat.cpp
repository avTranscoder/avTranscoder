#include "ProcessStat.hpp"

#include <utility>

namespace avtranscoder
{

void ProcessStat::addVideoStat( const size_t streamIndex, const VideoStat& videoStat )
{
	_videoStats.insert( std::make_pair( streamIndex, videoStat ) );
}

}
