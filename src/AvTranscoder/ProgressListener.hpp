#ifndef _AV_TRANSCODER_PROGRESS_LISTENER_HPP_
#define _AV_TRANSCODER_PROGRESS_LISTENER_HPP_

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
		std::cout << "c++ progress " << processedDuration << "/" << programDuration << std::endl;
		return eJobStatusContinue;
	}
};

}

#endif