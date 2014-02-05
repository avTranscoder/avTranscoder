#ifndef _AV_TRANSCODER_INPUT_STREAM_AUDIO_HPP_
#define _AV_TRANSCODER_INPUT_STREAM_AUDIO_HPP_

#include "InputStream.hpp"

namespace avtranscoder
{

class InputStreamAudio : public InputStream
{
public:
	InputStreamAudio( const std::string& filename = "", const size_t streamIndex = 0 )
		: InputStream( filename, streamIndex )
	{};

private:

};

}

#endif