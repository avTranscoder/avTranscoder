#ifndef _AV_TRANSCODER_NO_DISPLAY_PROGRESS_HPP_
#define _AV_TRANSCODER_NO_DISPLAY_PROGRESS_HPP_

#include "IProgress.hpp"

namespace avtranscoder
{

/**
 * @brief Implementation of IProgress, to manage cases when we need an IProgress but don't care of a progress bar.
 */
class AvExport NoDisplayProgress : public IProgress
{
public:
    ~NoDisplayProgress() {}

    EJobStatus progress(const double processedDuration, const double programDuration) { return eJobStatusContinue; }
};
}

#endif
