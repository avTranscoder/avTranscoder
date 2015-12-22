#include "ProcessStat.hpp"

#include <math.h>

namespace avtranscoder
{

double VideoStat::toPSNR(const double mse)
{
    return -10.0 * log10(mse);
}
}
