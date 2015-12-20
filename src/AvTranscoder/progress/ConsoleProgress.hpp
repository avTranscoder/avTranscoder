#ifndef _AV_TRANSCODER_CONSOLE_PROGRESS_HPP_
#define _AV_TRANSCODER_CONSOLE_PROGRESS_HPP_

#include "IProgress.hpp"

namespace avtranscoder
{

/**
 * @brief Implementation of IProgress, to display a progress bar in console.
 */
class AvExport ConsoleProgress : public IProgress
{
public:
    ~ConsoleProgress();

    EJobStatus progress(const double processedDuration, const double programDuration);
};
}

#endif
