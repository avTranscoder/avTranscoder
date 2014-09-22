#include "ProgressListener.hpp"

#include <iostream>
#include <iomanip>

namespace avtranscoder
{

IProgress::~IProgress()
{
}

ConsoleProgress::~ConsoleProgress()
{
}

EJobStatus ConsoleProgress::progress( const double processedDuration, const double programDuration )
{
	std::string progress( 80, '-' );
	std::string done( 80.0 * processedDuration / programDuration, '#' );
	progress.replace( 0, done.size(), done );

	std::cout  << std::setprecision(2) << std::fixed << "\r[" << progress << "] " << processedDuration << "/" << programDuration << std::flush;

	// if( processedFrames >= 100 )
	// 	return avtranscoder::eJobStatusCancel;

	return eJobStatusContinue;
}

NoDisplayProgress::~NoDisplayProgress()
{}

EJobStatus NoDisplayProgress::progress( const double processedDuration, const double programDuration )
{
	return eJobStatusContinue;
}

}
