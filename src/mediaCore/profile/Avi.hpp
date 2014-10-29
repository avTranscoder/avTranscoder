#ifndef _MEDIA_ENGINE_CORE_PROFILE_AVI_HPP_
#define _MEDIA_ENGINE_CORE_PROFILE_AVI_HPP_

namespace mediaengine
{
namespace mediacore
{

void loadAvi( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc avi;
	avi[ constants::mediaProfileIdentificator ] = "avi";
	avi[ constants::mediaProfileIdentificatorHuman ] = "AVI (Audio Video Interleaved)";
	avi[ constants::mediaProfileType ] = constants::mediaProfileTypeFormat;

	avi[ constants::mediaProfileFormat ] = "avi";

	profiles.push_back( avi );
}

}
}

#endif
