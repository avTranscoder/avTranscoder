#ifndef _MEDIA_ENGINE_CORE_PROGRESS_CONSOLE_PROGRESS_HPP_
#define _MEDIA_ENGINE_CORE_PROGRESS_CONSOLE_PROGRESS_HPP_

#include "IProgress.hpp"

namespace mediaengine
{
namespace mediacore
{

/**
 * @brief Implementation of IProgress, to display a progress bar in console.
 */
class MediaEngineExport ConsoleProgress : public IProgress
{
public:
	~ConsoleProgress();

	EJobStatus progress( const double processedDuration, const double programDuration );
};

}
}

#endif
