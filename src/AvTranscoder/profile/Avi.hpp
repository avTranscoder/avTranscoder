#ifndef _AVTRANSCODER_PROFIL_AVI_HPP_
#define _AVTRANSCODER_PROFIL_AVI_HPP_

namespace avtranscoder
{

void loadAvi( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc avi;
	avi[ Profile::avProfileIdentificator ] = "avi";
	avi[ Profile::avProfileIdentificatorHuman ] = "AVI (Audio Video Interleaved)";
	avi[ Profile::avProfileType ] = Profile::avProfileTypeFormat;

	avi[ Profile::avProfileFormat ] = "avi";

	profiles.push_back( avi );
}

}

#endif
