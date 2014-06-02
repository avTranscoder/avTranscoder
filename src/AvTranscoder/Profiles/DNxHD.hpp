#ifndef _AVTRANSCODER_PROFIL_DNXHD_HPP_
#define _AVTRANSCODER_PROFIL_DNXHD_HPP_

namespace avtranscoder
{

void loadDNxHD( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc dnxhd120;
	dnxhd120[ Profile::avProfilIdentificator ] = "dnxhd120";
	dnxhd120[ Profile::avProfilIdentificatorHuman ] = "DNxHD 120";
	dnxhd120[ "codec" ] = "dnxhd";
	dnxhd120[ "b" ] = "120000000";
	dnxhd120[ "pix_fmt" ] = "yuv422p";
	dnxhd120[ "g" ] = "1";

	Profile::ProfileDesc dnxhd185;
	dnxhd185[ Profile::avProfilIdentificator ] = "dnxhd185";
	dnxhd185[ Profile::avProfilIdentificatorHuman ] = "DNxHD 185";
	dnxhd185[ "codec" ] = "dnxhd";
	dnxhd185[ "b" ] = "185000000";
	dnxhd185[ "pix_fmt" ] = "yuv422p";
	dnxhd185[ "g" ] = "1";

	Profile::ProfileDesc dnxhd185x;
	dnxhd185x[ Profile::avProfilIdentificator ] = "dnxhd185x";
	dnxhd185x[ Profile::avProfilIdentificatorHuman ] = "DNxHD 185 X";
	dnxhd185x[ "codec" ] = "dnxhd";
	dnxhd185x[ "b" ] = "185000000";
	dnxhd185x[ "pix_fmt" ] = "yuv422p10";
	dnxhd185x[ "g" ] = "1";

	profiles.push_back( dnxhd120 );
	profiles.push_back( dnxhd185 );
	profiles.push_back( dnxhd185x );

}

}

#endif