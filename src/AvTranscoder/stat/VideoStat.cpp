#include "ProcessStat.hpp"

#include <math.h>

namespace avtranscoder
{

double VideoStat::psnr( const double d )
{
	return -10.0 * log(d) / log(10.0);
}

}
