#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP

#include "IEssenceTransform.hpp"

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/essenceStructures/Frame.hpp>

#ifdef AV_RESAMPLE_LIBRARY
 #define ResampleContext AVAudioResampleContext
#else
 #define ResampleContext SwrContext
#endif

class ResampleContext;

namespace avtranscoder
{

class AvExport AudioEssenceTransform : public IEssenceTransform
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
