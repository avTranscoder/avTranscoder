#ifndef _AV_TRANSCODER_ESSENCE_STREAM_IINPUT_ESSENCE_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_IINPUT_ESSENCE_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/frame/Frame.hpp>

namespace avtranscoder
{

class AvExport IInputEssence
{
public:
	virtual ~IInputEssence() {};

	/**
	 * @brief Open the decoder
	 */
	virtual void setup() = 0;

	/**
	 * @brief Decode next frame
	 * @param frameBuffer: the frame decoded
	 * @return status of decoding
	 */
	virtual bool readNextFrame( Frame& frameBuffer ) = 0;

	/**
	 * @brief Decode substream of next frame
	 * @param frameBuffer: the frame decoded
	 * @param subStreamIndex: index of substream to extract
	 * @return status of decoding
	 */
	virtual bool readNextFrame( Frame& frameBuffer, const size_t subStreamIndex ) = 0;

	/**
	 * Set the next frame of the input stream
	 * @note Not yet implemented for AvInputVideo and AvInputAudio
	 * @param inputFrame: the new next frame
	 */
	virtual void setFrame( Frame& inputFrame ) {}
};

}

#endif
