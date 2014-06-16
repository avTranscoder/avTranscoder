#ifndef _AVTRANSCODER_PROFIL_XDCAMHD422_HPP_
#define _AVTRANSCODER_PROFIL_XDCAMHD422_HPP_

namespace avtranscoder
{

void loadXdCamHD422( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc wave24bits;

	wave24bits[ Profile::avProfilIdentificator ] = "wave24bits";
	wave24bits[ Profile::avProfilIdentificatorHuman ] = "Wave 24bits";

	wave24bits[ "codec" ] = "pcm_s24le";
	wave24bits[ "sample_fmt" ] = "s32";

	Profile::ProfileDesc wave16bits;

	wave16bits[ Profile::avProfilIdentificator ] = "wave16bits";
	wave16bits[ Profile::avProfilIdentificatorHuman ] = "Wave 16bits";

	wave16bits[ "codec" ] = "pcm_s16le";
	wave16bits[ "sample_fmt" ] = "s16";
}

}

#endif
