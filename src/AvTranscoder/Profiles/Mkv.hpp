#ifndef _AVTRANSCODER_PROFIL_MKV_HPP_
#define _AVTRANSCODER_PROFIL_MKV_HPP_

namespace avtranscoder
{

void loadMkv( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc mkv;
	mkv[ Profile::avProfileIdentificator ] = "mkv";
	mkv[ Profile::avProfileIdentificatorHuman ] = "Matroska";
	mkv[ Profile::avProfileType ] = Profile::avProfileTypeFormat;

	mkv[ "format" ] = "mastroka";

	profiles.push_back( mkv );
}

}

#endif
