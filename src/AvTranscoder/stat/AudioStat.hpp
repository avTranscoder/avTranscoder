#ifndef  _AV_TRANSCODER_AUDIOSTAT_HPP
#define  _AV_TRANSCODER_AUDIOSTAT_HPP

#include <AvTranscoder/common.hpp>

namespace avtranscoder
{

/**
 * @brief Statistics related to an audio stream.
 */
class AvExport AudioStat
{
public:
	AudioStat( const double duration, const size_t nbPackets )
	: _duration( duration )
	, _nbPackets( nbPackets )
	{}

public:
	double _duration;
	size_t _nbPackets;
};

}

#endif
