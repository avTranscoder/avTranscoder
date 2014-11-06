#ifndef _AVTRANSCODER_PROFIL_MKV_HPP_
#define _AVTRANSCODER_PROFIL_MKV_HPP_

namespace avtranscoder
{

void loadMkv( ProfileLoader::Profiles& profiles )
{
	ProfileLoader::Profile mkv;
	mkv[ constants::avProfileIdentificator ] = "mkv";
	mkv[ constants::avProfileIdentificatorHuman ] = "Matroska";
	mkv[ constants::avProfileType ] = constants::avProfileTypeFormat;

	mkv[ constants::avProfileFormat ] = "matroska";

	profiles.push_back( mkv );
}

}

#endif
