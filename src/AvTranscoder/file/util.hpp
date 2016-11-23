#ifndef _AV_TRANSCODER_FILE_UTIL_HPP_
#define _AV_TRANSCODER_FILE_UTIL_HPP_

namespace avtranscoder
{

/**
 * @brief Level of file analysis.
 */
enum EAnalyseLevel
{
    eAnalyseLevelHeader = 0,
    eAnalyseLevelFirstGop = 1,
    eAnalyseLevelFull = 2
};
}

#endif
