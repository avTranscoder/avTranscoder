#ifndef _AV_TRANSCODER_PROGRESS_LISTENER_HPP_
#define _AV_TRANSCODER_PROGRESS_LISTENER_HPP_

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
	virtual ~IProgress() = 0;
	
	/**
	 * @brief Manage the progress.
	 * @param processedDuration: what is processed
	 * @param programDuration: what you need to process (the totality)
	 * @return eJobStatusContinue if we continue the progress or eJobStatusCancel to stop it.
	 */
	virtual EJobStatus progress( const double processedDuration, const double programDuration ) = 0;
};

/**
 * @brief Implementation of IProgress, to display a progress bar in console.
 */
class ConsoleProgress : public IProgress
{
public:
	~ConsoleProgress();

	EJobStatus progress( const double processedDuration, const double programDuration );
};

/**
 * @brief Implementation of IProgress, to manage cases when we need an IProgress but don't care of a progress bar.
 */
class NoDisplayProgress : public IProgress
{
public:
	~NoDisplayProgress();

	EJobStatus progress( const double processedDuration, const double programDuration );
};

}

#endif