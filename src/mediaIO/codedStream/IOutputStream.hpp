#ifndef _MEDIA_MANAGER_IO_CODED_STREAM_I_OUTPUT_STREAM_HPP_
#define _MEDIA_MANAGER_IO_CODED_STREAM_I_OUTPUT_STREAM_HPP_

#include <mediaCore/frame/Frame.hpp>

namespace mediamanager
{
namespace mediaio
{

class IOutputStream
{
public:
	virtual ~IOutputStream() {};

	virtual size_t getStreamIndex() const = 0;

	virtual bool wrap( const mediacore::CodedData& data ) = 0;
};

}
}

#endif