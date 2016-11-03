#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_ESSENCE_TRANSFORM_HPP_
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_ESSENCE_TRANSFORM_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/data/decoded/IFrame.hpp>

namespace avtranscoder
{

class AvExport ITransform
{
public:
    ITransform() {}

    virtual ~ITransform() {}

    virtual void convert(const IFrame& src, IFrame& dst) = 0;
};
}

#endif
