#ifndef _AV_TRANSCODER_ESSENCE_STREAM_IOUTPUT_ESSENCE_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_IOUTPUT_ESSENCE_HPP_

#include <AvTranscoder/essenceStructures/Frame.hpp>
#include <AvTranscoder/codec/DataStream.hpp>
#include <AvTranscoder/codec/ICodec.hpp>

namespace avtranscoder
{

class AvExport IOutputEssence
{
public:
	IOutputEssence( const std::string& codecName )
	: _codedDesc( codecName )
	{}

	virtual ~IOutputEssence()
	{}

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
	virtual bool encodeFrame( const Frame& sourceFrame, DataStream& codedFrame ) = 0;

	/**
	 * @brief Get delayed encoded frames
	 * @param codedFrame data of the coded frame if present (first frames can be delayed)
	 * @return status of encoding
	 */
	virtual bool encodeFrame( DataStream& codedFrame ) = 0;

	ICodec& getCodedDesc() { return _codedDesc; }

protected:
	ICodec _codedDesc;

};

}

#endif