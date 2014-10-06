#ifndef _AV_TRANSCODER_ESSENCE_TRANSFORM_ESSENCE_TRANSFORM_HPP_
#define _AV_TRANSCODER_ESSENCE_TRANSFORM_ESSENCE_TRANSFORM_HPP_

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/essenceStructures/Frame.hpp>

namespace avtranscoder
{

class AvExport IEssenceTransform
{
public:
	IEssenceTransform()
	{}

	virtual ~IEssenceTransform()
	{}

	virtual void convert( const Frame& src, Frame& dst ) = 0;

protected:
	virtual bool init( const Frame& src, const Frame& dst ) = 0;
};

}

#endif
