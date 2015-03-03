#ifndef _AV_TRANSCODER_ENCODER_AUDIO_ENCODER_HPP_
#define _AV_TRANSCODER_ENCODER_AUDIO_ENCODER_HPP_

#include "IEncoder.hpp"
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport AudioEncoder : public IEncoder
{
public:
	AudioEncoder( const std::string& audioCodecName );
	~AudioEncoder();
	
	void setup();

	bool encodeFrame( const Frame& sourceFrame, Frame& codedFrame );
	bool encodeFrame( Frame& codedFrame );

	ICodec& getCodec() { return _codec; }
	AudioCodec& getAudioCodec() { return _codec; }

	void setProfile( const ProfileLoader::Profile& profile, const AudioFrameDesc& frameDesc );

private:
	AudioCodec _codec;
	AVFrame* _frame;  ///< Contains the encoded data to pass to the Frame when encodeFrame
};

}

#endif