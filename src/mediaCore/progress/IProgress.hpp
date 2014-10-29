#ifndef _MEDIA_ENGINE_CORE_PROGRESS_IPROGRESS_HPP_
#define _MEDIA_ENGINE_CORE_PROGRESS_IPROGRESS_HPP_

#include <mediaCore/common.hpp>

#include <iostream>

namespace mediaengine
{
namespace mediacore
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
class MediaEngineExport IProgress
{
public:
	virtual ~IProgress() {};
	
	/**
	 * @brief Manage the progress.
	 * @param processedDuration: what is processed
	 * @param programDuration: what you need to process (the totality)
	 * @return return EJobStatus to manage the process (continuing, stopping, paused, etc)
	 */
	virtual EJobStatus progress( const double processedDuration, const double programDuration ) = 0;
};

}
}

#endif
