#ifndef _AV_TRANSCODER_PROFIL_AVI_HPP_
#define _AV_TRANSCODER_PROFIL_AVI_HPP_

namespace avtranscoder
{

void loadAvi( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc avi;
	avi[ constants::avProfileIdentificator ] = "avi";
	avi[ constants::avProfileIdentificatorHuman ] = "AVI (Audio Video Interleaved)";
	avi[ constants::avProfileType ] = constants::avProfileTypeFormat;

	avi[ constants::avProfileFormat ] = "avi";

	profiles.push_back( avi );
}

}

#endif
