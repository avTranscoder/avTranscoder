#ifndef _AV_TRANSCODER_ESSENCE_STREAM_IENCODER_HPP_
#define _AV_TRANSCODER_ESSENCE_STREAM_IENCODER_HPP_

#include <AvTranscoder/data/decoded/Frame.hpp>
#include <AvTranscoder/data/coded/CodedData.hpp>
#include <AvTranscoder/codec/ICodec.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>

namespace avtranscoder
{

class AvExport IEncoder
{
public:
    virtual ~IEncoder() {}

    /**
     * @brief Setup the encoder
     * @param profile: set encoder parameters from the given profile
     * @note Open the encoder.
     */
    virtual void setupEncoder(const ProfileLoader::Profile& profile = ProfileLoader::Profile()) = 0;

    /**
     * @brief Encode a new frame, and get coded frame
     * @param sourceFrame: frame that needs to be encoded
     * @param codedFrame: output encoded coded data (first frames can be delayed)
     * @return status of encoding
     * @throw runtime_error if the encoded process failed.
     */
    virtual bool encodeFrame(const Frame& sourceFrame, CodedData& codedFrame) = 0;

    /**
     * @brief Get delayed encoded frames
     * @param codedFrame: output encoded coded data of last frames
     * @return status of encoding
     * @throw runtime_error if the encoded process failed.
     */
    virtual bool encodeFrame(CodedData& codedFrame) = 0;

    /**
     * @brief Get codec used for encoding.
     * @return a reference to the codec
     */
    virtual ICodec& getCodec() = 0;
};
}

#endif
