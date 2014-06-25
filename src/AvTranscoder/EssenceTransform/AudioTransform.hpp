#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_TRANSFORM_HPP

#include <AvTranscoder/common.hpp>
#include "EssenceTransform.hpp"

#ifdef AV_RESAMPLE_LIBRARY
 #define ResampleContext AVAudioResampleContext
#else
 #define ResampleContext SwrContext
#endif

class ResampleContext;

namespace avtranscoder
{

class AvExport AudioTransform : public EssenceTransform
{
public:
	AudioTransform();

	void convert( const Frame& src, Frame& dst );

private:
	bool init( const Frame& src, const Frame& dst );
	
	ResampleContext* _audioConvertContext;

	bool _isInit;
};

}

#endif
