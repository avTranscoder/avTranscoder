#ifndef _MEDIA_MANAGER_MEDIA_TRANSFORM_I_TRANSFORM_HPP_
#define _MEDIA_MANAGER_MEDIA_TRANSFORM_I_TRANSFORM_HPP_

#include <mediaCore/common.hpp>
#include <mediaCore/frame/Frame.hpp>

namespace mediamanager
{
namespace mediatransform
{

class MediaManagerExport ITransform
{
public:
	ITransform()
	{}

	virtual ~ITransform()
	{}

	virtual void convert( const mediacore::Frame& src, mediacore::Frame& dst ) = 0;

protected:
	virtual bool init( const mediacore::Frame& src, const mediacore::Frame& dst ) = 0;
};

}
}

#endif
