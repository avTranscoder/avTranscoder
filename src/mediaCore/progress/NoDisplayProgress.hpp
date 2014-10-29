#ifndef _MEDIA_ENGINE_CORE_PROGRESS_NO_DISPLAY_PROGRESS_HPP_
#define _MEDIA_ENGINE_CORE_PROGRESS_NO_DISPLAY_PROGRESS_HPP_

#include "IProgress.hpp"

namespace mediaengine
{
namespace mediacore
{

/**
 * @brief Implementation of IProgress, to manage cases when we need an IProgress but don't care of a progress bar.
 */
class MediaEngineExport NoDisplayProgress : public IProgress
{
public:
	~NoDisplayProgress()
	{}

	EJobStatus progress( const double processedDuration, const double programDuration )
	{
		return eJobStatusContinue;
	}
};

}
}

#endif
