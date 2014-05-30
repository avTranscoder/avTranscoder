
#include <iostream>
#include <iomanip>
#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/OutputStreamVideo.hpp>


int main( int argc, char** argv )
{
	avtranscoder::Profile p;
	p.loadProfiles();

	std::cout << p.getProfiles().size() << std::endl;

	std::cout << std::left;

	for( auto profile : p.getProfiles() )
	{
		std::cout << "********************" << std::endl;
		for( auto key : profile )
			std::cout << std::setw(35) << key.first << key.second << std::endl;

		avtranscoder::OutputStreamVideo outputVideoStream;
		outputVideoStream.setProfile( profile[ "avProfile" ] );
	}

}