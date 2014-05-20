#ifndef _AV_TRANSCODER_PROGRESS_LISTENER_HPP_
#define _AV_TRANSCODER_PROGRESS_LISTENER_HPP_

#include <iostream>
#include <iomanip>

namespace avtranscoder
{

enum EJobStatus
{
	eJobStatusContinue = 0,
	eJobStatusCancel
};

class ProgressListener
{
public:
	ProgressListener()
	{}

	virtual ~ProgressListener()
	{}

	virtual EJobStatus progress( const double processedDuration, const double programDuration )
	{
		std::string progress( 80, '-' );
		std::string done( 80.0 * processedDuration / programDuration, '#' );
		progress.replace( 0, done.size(), done );

		std::cout  << std::setprecision(2) << std::fixed << "\r[" << progress << "] " << processedDuration << "/" << programDuration << std::flush;

		// if( processedFrames >= 100 )
		// 	return avtranscoder::eJobStatusCancel;

		return eJobStatusContinue;
	}
};

}

#endif