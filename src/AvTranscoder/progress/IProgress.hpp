#ifndef _AV_TRANSCODER_IPROGRESS_HPP_
#define _AV_TRANSCODER_IPROGRESS_HPP_

namespace avtranscoder
{

/**
 * @brief Indicate the state of a process.
 */
enum EJobStatus
{
	eJobStatusContinue = 0,
	eJobStatusCancel
};

/**
 * @brief Base class of Progress.
 * Inherit this class to have your own way to manage a progress bar.
 * You can inherit this class in C++, but also in python / Java binding.
 */
class IProgress
{
public:
	virtual ~IProgress() {};
	
	/**
	 * @brief Manage the progress.
	 * @param processedDuration: what is processed
	 * @param programDuration: what you need to process (the totality)
	 * @return eJobStatusContinue if we continue the progress or eJobStatusCancel to stop it.
	 */
	virtual EJobStatus progress( const double processedDuration, const double programDuration ) = 0;
};

}

#endif
