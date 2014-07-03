#ifndef _AVTRANSCODER_PROFIL_DNXHD_HPP_
#define _AVTRANSCODER_PROFIL_DNXHD_HPP_

namespace avtranscoder
{

void loadDNxHD( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc dnxhd120;
	dnxhd120[ Profile::avProfileIdentificator ] = "dnxhd120";
	dnxhd120[ Profile::avProfileIdentificatorHuman ] = "DNxHD 120";
	dnxhd120[ Profile::avProfileType ] = Profile::avProfileTypeVideo;
	dnxhd120[ Profile::avProfileCodec ] = "dnxhd";
	dnxhd120[ "b" ] = "120000000";
	dnxhd120[ Profile::avProfilePixelFormat ] = "yuv422p";
	dnxhd120[ "g" ] = "1";
	dnxhd120[ Profile::avProfileFrameRate ] = "25";

	Profile::ProfileDesc dnxhd185;
	dnxhd185[ Profile::avProfileIdentificator ] = "dnxhd185";
	dnxhd185[ Profile::avProfileIdentificatorHuman ] = "DNxHD 185";
	dnxhd185[ Profile::avProfileType ] = Profile::avProfileTypeVideo;
	dnxhd185[ Profile::avProfileCodec ] = "dnxhd";
	dnxhd185[ "b" ] = "185000000";
	dnxhd185[ Profile::avProfilePixelFormat ] = "yuv422p";
	dnxhd185[ "g" ] = "1";
	dnxhd185[ Profile::avProfileFrameRate ] = "25";

	Profile::ProfileDesc dnxhd185x;
	dnxhd185x[ Profile::avProfileIdentificator ] = "dnxhd185x";
	dnxhd185x[ Profile::avProfileIdentificatorHuman ] = "DNxHD 185 X";
	dnxhd185x[ Profile::avProfileType ] = Profile::avProfileTypeVideo;
	dnxhd185x[ Profile::avProfileCodec ] = "dnxhd";
	dnxhd185x[ "b" ] = "185000000";
	dnxhd185x[ Profile::avProfilePixelFormat ] = "yuv422p10";
	dnxhd185x[ "g" ] = "1";
	dnxhd185x[ Profile::avProfileFrameRate ] = "25";

	profiles.push_back( dnxhd120 );
	profiles.push_back( dnxhd185 );
	profiles.push_back( dnxhd185x );

}

}

#endif