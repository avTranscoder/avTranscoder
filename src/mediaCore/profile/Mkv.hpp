#ifndef _MEDIA_ENGINE_CORE_PROFILE_MKV_HPP_
#define _MEDIA_ENGINE_CORE_PROFILE_MKV_HPP_

namespace mediaengine
{
namespace mediacore
{

void loadMkv( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc mkv;
	mkv[ constants::mediaProfileIdentificator ] = "mkv";
	mkv[ constants::mediaProfileIdentificatorHuman ] = "Matroska";
	mkv[ constants::mediaProfileType ] = constants::mediaProfileTypeFormat;

	mkv[ constants::mediaProfileFormat ] = "matroska";

	profiles.push_back( mkv );
}

}
}

#endif
