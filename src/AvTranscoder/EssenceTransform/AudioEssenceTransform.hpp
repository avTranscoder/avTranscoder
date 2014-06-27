#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP

#include <AvTranscoder/common.hpp>

#ifdef AV_RESAMPLE_LIBRARY
 #define ResampleContext AVAudioResampleContext
#else
 #define ResampleContext SwrContext
#endif

class ResampleContext;

namespace avtranscoder
{

class AudioFrame;

class AvExport AudioEssenceTransform
{
public:
	AudioEssenceTransform();

	void convert( const AudioFrame& src, AudioFrame& dst );

private:
	bool init( const AudioFrame& src, const AudioFrame& dst );
	
	ResampleContext* _audioConvertContext;

	bool _isInit;
};

}

#endif
