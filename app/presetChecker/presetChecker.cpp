
#include <iostream>
#include <iomanip>
#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/OutputStreamVideo.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>


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
			avtranscoder::OutputStreamVideo outputVideoStream;
			outputVideoStream.setProfile( profile.find( avtranscoder::Profile::avProfileIdentificator )->second );
		}

		if( profile.find( avtranscoder::Profile::avProfileType )->second == avtranscoder::Profile::avProfileTypeAudio )
		{
			avtranscoder::OutputStreamAudio outputAudioStream;
			outputAudioStream.setProfile( profile.find( avtranscoder::Profile::avProfileIdentificator )->second );
		}

	}

}