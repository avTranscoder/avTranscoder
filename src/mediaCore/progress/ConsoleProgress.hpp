#ifndef _MEDIA_MANAGER_CORE_PROGRESS_CONSOLE_PROGRESS_HPP_
#define _MEDIA_MANAGER_CORE_PROGRESS_CONSOLE_PROGRESS_HPP_

#include "IProgress.hpp"

namespace mediamanager
{
namespace mediacore
{

/**
 * @brief Implementation of IProgress, to display a progress bar in console.
 */
class MediaManagerExport ConsoleProgress : public IProgress
{
public:
	~ConsoleProgress();

	EJobStatus progress( const double processedDuration, const double programDuration );
};

}
}

#endif
