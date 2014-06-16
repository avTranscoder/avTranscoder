#ifndef _AV_TRANSCODER_AUDIO_TRANSFORM_HPP
#define _AV_TRANSCODER_AUDIO_TRANSFORM_HPP

#include "common.hpp"

class SwrContext;

namespace avtranscoder
{

class AudioFrame;

class AvExport AudioTransform
{
public:
	AudioTransform();

	void convert( const AudioFrame& src, AudioFrame& dst );

private:
	bool init( const AudioFrame& src, const AudioFrame& dst );
	
	SwrContext* m_audioConvertContext;

	bool m_isInit;
};

}

#endif
