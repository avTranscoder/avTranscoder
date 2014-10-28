
#include <mediaCore/profile/Profile.hpp>

#include <mediaIO/essenceStream/AvOutputVideo.hpp>
#include <mediaIO/essenceStream/AvOutputAudio.hpp>

#include <iostream>
#include <iomanip>

int main( int argc, char** argv )
{
	using namespace mediamanager;

	mediacore::Profile p;
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
			using namespace mediamanager::mediacore::constants;
			if( profile.find( mediaProfileType )->second == mediaProfileTypeVideo )
			{
				mediaio::AvOutputVideo outputVideo;
				outputVideo.setProfile( profile, outputVideo.getVideoCodec().getVideoFrameDesc() );
			}

			if( profile.find( mediaProfileType )->second == mediaProfileTypeAudio )
			{
				mediaio::AvOutputAudio outputAudio;
				outputAudio.setProfile( profile, outputAudio.getAudioCodec().getFrameDesc() );
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
