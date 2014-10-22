#ifndef _AV_TRANSCODER_PROFIL_MKV_HPP_
#define _AV_TRANSCODER_PROFIL_MKV_HPP_

namespace avtranscoder
{

void loadMkv( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc mkv;
	mkv[ constants::avProfileIdentificator ] = "mkv";
	mkv[ constants::avProfileIdentificatorHuman ] = "Matroska";
	mkv[ constants::avProfileType ] = constants::avProfileTypeFormat;

	mkv[ constants::avProfileFormat ] = "matroska";

	profiles.push_back( mkv );
}

}

#endif
