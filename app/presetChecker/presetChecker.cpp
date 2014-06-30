
#include <iostream>
#include <iomanip>
#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/EssenceStream/OutputVideo.hpp>
#include <AvTranscoder/EssenceStream/OutputAudio.hpp>


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

		if( profile.find( avtranscoder::Profile::avProfileType )->second == avtranscoder::Profile::avProfileTypeVideo )
		{
			avtranscoder::OutputVideo outputVideo;
			outputVideo.setProfile( profile );
		}

		if( profile.find( avtranscoder::Profile::avProfileType )->second == avtranscoder::Profile::avProfileTypeAudio )
		{
			avtranscoder::OutputAudio outputAudio;
			outputAudio.setProfile( profile );
		}

	}

}