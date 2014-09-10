#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP

#include "EssenceTransform.hpp"

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/EssenceStructures/Frame.hpp>

#ifdef AV_RESAMPLE_LIBRARY
 #define ResampleContext AVAudioResampleContext
#else
 #define ResampleContext SwrContext
#endif

class ResampleContext;

namespace avtranscoder
{

class AvExport AudioEssenceTransform : public EssenceTransform
{
public:
	AudioEssenceTransform();

	void convert( const Frame& srcFrame, Frame& dstFrame );

private:
	bool init( const Frame& srcFrame, const Frame& dstFrame );
	
	ResampleContext* _audioConvertContext;

	bool _isInit;
};

}

#endif
