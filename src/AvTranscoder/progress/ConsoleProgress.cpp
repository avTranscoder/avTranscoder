#include "ConsoleProgress.hpp"

#include <iostream>
#include <iomanip>

namespace avtranscoder
{

ConsoleProgress::~ConsoleProgress()
{
}

EJobStatus ConsoleProgress::progress( const double processedDuration, const double programDuration )
{
	// The program could be an image (programDuration = 0)
	double progressDuration = programDuration ? processedDuration / programDuration : 0;

	std::string progress( 80, '-' );
	std::string done( 80.0 * progressDuration, '#' );
	progress.replace( 0, done.size(), done );

	std::cout  << std::setprecision(2) << std::fixed << "\r[" << progress << "] " << processedDuration << "/" << programDuration << std::flush;

	return eJobStatusContinue;
}

}
