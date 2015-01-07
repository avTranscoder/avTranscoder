#ifndef _AV_TRANSCODER_FORMAT_CONTEXT_HPP_
#define _AV_TRANSCODER_FORMAT_CONTEXT_HPP_

#include "Context.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

namespace avtranscoder
{

/**
 * @brief Wrapper of an AVFormatContext.
 */
class FormatContext : public Context
{
public:
	FormatContext( const std::string& filename, int req_flags = 0 );  ///< Allocate an AVFormatContext by opening an input file
	FormatContext( int req_flags = 0 );  ///< Allocate an AVFormatContext with default values
	~FormatContext();

	void findStreamInfo( AVDictionary** options = NULL );  ///< Read packets of a media file to get stream information.

	size_t getNbStreams() const { return getAVFormatContext().nb_streams; }
	size_t getDuration() const { return getAVFormatContext().duration; }
	size_t getStartTime() const { return getAVFormatContext().start_time; }

#ifndef SWIG
	AVFormatContext& getAVFormatContext() const { return *static_cast<AVFormatContext*>( _avContext ); }
	AVStream& getAVStream( size_t index ) const;
#endif

private:
	bool _isOpen;  ///< Is the AVFormatContext open (in constructor with a filename)
};

}

#endif
