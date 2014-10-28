#ifndef _MEDIA_MANAGER_IO_ESSENCE_STREAM_I_OUTPUT_ESSENCE_HPP_
#define _MEDIA_MANAGER_IO_ESSENCE_STREAM_I_OUTPUT_ESSENCE_HPP_

#include <mediaCore/common.hpp>
#include <mediaCore/frame/Frame.hpp>
#include <mediaIO/codec/ICodec.hpp>

namespace mediamanager
{
namespace mediaio
{

class MediaManagerExport IOutputEssence
{
public:
	virtual ~IOutputEssence() {}

	/**
	 * @brief Setup the encoder
	 */
	virtual void setup() = 0;

	/**
	 * @brief Encode a new frame, and get coded frame
	 * @param sourceFrame frame need to be encoded
	 * @param codedFrame data of the coded frame if present (first frames can be delayed)
	 * @return status of encoding
	 */
	virtual bool encodeFrame( const mediacore::Frame& sourceFrame, mediacore::Frame& codedFrame ) = 0;

	/**
	 * @brief Get delayed encoded frames
	 * @param codedFrame data of the coded frame if present (first frames can be delayed)
	 * @return status of encoding
	 */
	virtual bool encodeFrame( mediacore::Frame& codedFrame ) = 0;

	/**
	 * @brief Get codec used for encoding.
	 * @return a reference to the codec
	 */
	virtual ICodec& getCodec() = 0;
};

}
}

#endif
