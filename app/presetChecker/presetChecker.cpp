
#include <iostream>
#include <iomanip>
#include <AvTranscoder/Profile.hpp>

#include <AvTranscoder/essenceStream/AvOutputVideo.hpp>
#include <AvTranscoder/essenceStream/AvOutputAudio.hpp>


int main( int argc, char** argv )
{
	avtranscoder::Profile p;
	p.loadProfiles();

	std::cout << p.getProfiles().size() << std::endl;

	size_t presetInError = 0;

	std::cout << std::left;

	for( auto profile : p.getProfiles() )
	{
		std::cout << "********************" << std::endl;
		for( auto key : profile )
			std::cout << std::setw(35) << key.first << key.second << std::endl;

		try{
			if( profile.find( avtranscoder::Profile::avProfileType )->second == avtranscoder::Profile::avProfileTypeVideo )
			{
				avtranscoder::AvOutputVideo outputVideo;
				outputVideo.setProfile( profile, outputVideo.getVideoDesc().getVideoFrameDesc() );
			}

			if( profile.find( avtranscoder::Profile::avProfileType )->second == avtranscoder::Profile::avProfileTypeAudio )
			{
				avtranscoder::AvOutputAudio outputAudio;
				outputAudio.setProfile( profile, outputAudio.getAudioDesc().getFrameDesc() );
			}
		}
		catch( ... )
		{
			std::cout << "ERROR on preset ! " << std::endl;
			presetInError ++;
		}
	}

	std::cout << "********************" << std::endl;
	std::cout << " Result: " << std::endl;
	if( presetInError )
	{
		std::cout << presetInError << " / " << p.getProfiles().size() << " are incorrects" << std::endl;
	}
	else
	{
		std::cout << "every presets are corrects" << std::endl;
	}
	std::cout << "********************" << std::endl;
}
