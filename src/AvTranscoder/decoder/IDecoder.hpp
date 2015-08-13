#ifndef _AV_TRANSCODER_ESSENCE_STREAM_IDECODER_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_IDECODER_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/frame/Frame.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport IDecoder
{
public:
	virtual ~IDecoder() {};

	/**
	 * @brief Setup the decoder
	 * @param profile: set decoder parameters from the given profile
	 * @note Open the decoder.
	 */
	virtual void setupDecoder( const ProfileLoader::Profile& profile = ProfileLoader::Profile() ) {}

	/**
	 * @brief Decode next frame
	 * @param frameBuffer: the frame decoded
	 * @return status of decoding
	 */
	virtual bool decodeNextFrame( Frame& frameBuffer ) = 0;

	/**
	 * @brief Decode substream of next frame
	 * @param frameBuffer: the frame decoded
	 * @param subStreamIndex: index of substream to extract
	 * @return status of decoding
	 */
	virtual bool decodeNextFrame( Frame& frameBuffer, const size_t subStreamIndex ) = 0;

	/**
	 * @brief Set the next frame of the input stream (which bypass the work of decoding)
	 * @note Not yet implemented for VideoDecoder and AudioDecoder
	 * @param inputFrame: the new next frame
	 */
	virtual void setNextFrame( Frame& inputFrame ) {}
};

}

#endif
