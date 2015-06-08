#include "ProcessStat.hpp"

#include <utility>
#include <math.h>

namespace avtranscoder
{

double VideoStat::psnr( const double d )
{
	return -10.0 * log(d) / log(10.0);
}

void ProcessStat::addVideoStat( const size_t streamIndex, const VideoStat& videoStat )
{
	_videoStats.insert( std::make_pair( streamIndex, videoStat ) );
}

}
