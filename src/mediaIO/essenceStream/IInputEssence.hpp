#ifndef _MEDIA_MANAGER_IO_ESSENCE_STREAM_I_INPUT_ESSENCE_HPP_
#define _MEDIA_MANAGER_IO_ESSENCE_STREAM_I_INPUT_ESSENCE_HPP_

#include <mediaCore/common.hpp>
#include <mediaCore/frame/Frame.hpp>

namespace mediamanager
{
namespace mediaio
{

class MediaManagerExport IInputEssence
{
public:
	virtual ~IInputEssence() {};

	/**
	 * @brief Setup the decoder
	 */
	virtual void setup() = 0;

	/**
	 * @brief Decode next frame
	 * @param frameBuffer: the frame decoded
	 * @return status of decoding
	 */
	virtual bool readNextFrame( mediacore::Frame& frameBuffer ) = 0;

	/**
	 * @brief Decode substream of next frame
	 * @param frameBuffer: the frame decoded
	 * @param subStreamIndex: index of substream to extract
	 * @return status of decoding
	 */
	virtual bool readNextFrame( mediacore::Frame& frameBuffer, const size_t subStreamIndex ) = 0;
};

}
}

#endif
