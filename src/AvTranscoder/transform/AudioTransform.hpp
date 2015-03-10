#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_AUDIO_ESSENCE_TRANSFORM_HPP

#include "ITransform.hpp"

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/frame/Frame.hpp>

#ifdef AV_RESAMPLE_LIBRARY
 #define ResampleContext AVAudioResampleContext
#else
 #define ResampleContext SwrContext
#endif

class ResampleContext;

namespace avtranscoder
{

class AvExport AudioTransform : public ITransform
{
public:
	AudioTransform();
	~AudioTransform();

	void convert( const Frame& srcFrame, Frame& dstFrame );

private:
	bool init( const Frame& srcFrame, const Frame& dstFrame );

	/// Resize output buffer if source has a different size from the last process
	void initFrames( const Frame& srcFrame, Frame& dstFrame );

private:
	ResampleContext* _audioConvertContext;

	size_t _nbSamplesOfPreviousFrame;  ///< To check if the number of samples change between frames

	bool _isInit;
};

}

#endif
