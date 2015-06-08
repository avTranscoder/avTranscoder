#ifndef _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_
#define _AV_TRANSCODER_DECODER_VIDEO_GENERATOR_HPP_

#include "IDecoder.hpp"
#include <AvTranscoder/codec/VideoCodec.hpp>

namespace avtranscoder
{

class AvExport VideoGenerator : public IDecoder
{
public:
	VideoGenerator();
	VideoGenerator( const VideoGenerator& videoGenerator );
	VideoGenerator& operator=( const VideoGenerator& videoGenerator );

	~VideoGenerator();

	void setupDecoder( const ProfileLoader::Profile& profile = ProfileLoader::Profile() ) {}

	bool decodeNextFrame( Frame& frameBuffer );
	bool decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex );

	const VideoFrameDesc& getVideoFrameDesc() const { return _frameDesc; }
	void setVideoFrameDesc( const VideoFrameDesc& frameDesc );
	void setNextFrame( Frame& inputFrame );

private:
	Frame* _inputFrame;  ///< A frame given from outside (has link, no ownership)
	VideoFrame* _blackImage;   ///< The generated black image (has ownership)
	VideoFrameDesc _frameDesc;  ///< The description of the black image (width, height...) 
};

}

#endif