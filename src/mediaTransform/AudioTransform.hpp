#ifndef _MEDIA_ENGINE_TRANSFORM_AUDIO_TRANSFORM_HPP
#define _MEDIA_ENGINE_TRANSFORM_AUDIO_TRANSFORM_HPP

#include "ITransform.hpp"

#include <mediaCore/common.hpp>
#include <mediaCore/frame/Frame.hpp>

#ifdef AV_RESAMPLE_LIBRARY
 #define ResampleContext AVAudioResampleContext
#else
 #define ResampleContext SwrContext
#endif

class ResampleContext;

namespace mediaengine
{
namespace mediatransform
{

class MediaEngineExport AudioTransform : public ITransform
{
public:
	AudioTransform();

	void convert( const mediacore::Frame& srcFrame, mediacore::Frame& dstFrame );

private:
	bool init( const mediacore::Frame& srcFrame, const mediacore::Frame& dstFrame );
	
	ResampleContext* _audioConvertContext;

	bool _isInit;
};

}
}

#endif
