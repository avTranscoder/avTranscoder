#ifndef _AVTRANSCODER_PROFIL_DNXHD_HPP_
#define _AVTRANSCODER_PROFIL_DNXHD_HPP_

namespace avtranscoder
{

void loadDNxHD( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc dnxhd120;
	dnxhd120[ constants::avProfileIdentificator ] = "dnxhd120";
	dnxhd120[ constants::avProfileIdentificatorHuman ] = "DNxHD 120";
	dnxhd120[ constants::avProfileType ] = constants::avProfileTypeVideo;
	dnxhd120[ constants::avProfileCodec ] = "dnxhd";
	dnxhd120[ "b" ] = "120000000";
	dnxhd120[ constants::avProfilePixelFormat ] = "yuv422p";
	dnxhd120[ "g" ] = "1";
	dnxhd120[ constants::avProfileFrameRate ] = "25";

	Profile::ProfileDesc dnxhd185;
	dnxhd185[ constants::avProfileIdentificator ] = "dnxhd185";
	dnxhd185[ constants::avProfileIdentificatorHuman ] = "DNxHD 185";
	dnxhd185[ constants::avProfileType ] = constants::avProfileTypeVideo;
	dnxhd185[ constants::avProfileCodec ] = "dnxhd";
	dnxhd185[ "b" ] = "185000000";
	dnxhd185[ constants::avProfilePixelFormat ] = "yuv422p";
	dnxhd185[ "g" ] = "1";
	dnxhd185[ constants::avProfileFrameRate ] = "25";

	Profile::ProfileDesc dnxhd185x;
	dnxhd185x[ constants::avProfileIdentificator ] = "dnxhd185x";
	dnxhd185x[ constants::avProfileIdentificatorHuman ] = "DNxHD 185 X";
	dnxhd185x[ constants::avProfileType ] = constants::avProfileTypeVideo;
	dnxhd185x[ constants::avProfileCodec ] = "dnxhd";
	dnxhd185x[ "b" ] = "185000000";
	dnxhd185x[ constants::avProfilePixelFormat ] = "yuv422p10";
	dnxhd185x[ "g" ] = "1";
	dnxhd185x[ constants::avProfileFrameRate ] = "25";

	profiles.push_back( dnxhd120 );
	profiles.push_back( dnxhd185 );
	profiles.push_back( dnxhd185x );

}

}

#endif
