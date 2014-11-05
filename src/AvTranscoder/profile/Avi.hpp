#ifndef _AVTRANSCODER_PROFIL_AVI_HPP_
#define _AVTRANSCODER_PROFIL_AVI_HPP_

namespace avtranscoder
{

void loadAvi( ProfileLoader::Profiles& profiles )
{
	ProfileLoader::Profile avi;
	avi[ constants::avProfileIdentificator ] = "avi";
	avi[ constants::avProfileIdentificatorHuman ] = "AVI (Audio Video Interleaved)";
	avi[ constants::avProfileType ] = constants::avProfileTypeFormat;

	avi[ constants::avProfileFormat ] = "avi";

	profiles.push_back( avi );
}

}

#endif
