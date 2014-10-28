#ifndef _MEDIA_MANAGER_CORE_PROFILE_DNXHD_HPP_
#define _MEDIA_MANAGER_CORE_PROFILE_DNXHD_HPP_

namespace mediamanager
{
namespace mediacore
{

void loadDNxHD( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc dnxhd120;
	dnxhd120[ constants::mediaProfileIdentificator ] = "dnxhd120";
	dnxhd120[ constants::mediaProfileIdentificatorHuman ] = "DNxHD 120";
	dnxhd120[ constants::mediaProfileType ] = constants::mediaProfileTypeVideo;
	dnxhd120[ constants::mediaProfileCodec ] = "dnxhd";
	dnxhd120[ "b" ] = "120000000";
	dnxhd120[ constants::mediaProfilePixelFormat ] = "yuv422p";
	dnxhd120[ "g" ] = "1";
	dnxhd120[ constants::mediaProfileFrameRate ] = "25";

	Profile::ProfileDesc dnxhd185;
	dnxhd185[ constants::mediaProfileIdentificator ] = "dnxhd185";
	dnxhd185[ constants::mediaProfileIdentificatorHuman ] = "DNxHD 185";
	dnxhd185[ constants::mediaProfileType ] = constants::mediaProfileTypeVideo;
	dnxhd185[ constants::mediaProfileCodec ] = "dnxhd";
	dnxhd185[ "b" ] = "185000000";
	dnxhd185[ constants::mediaProfilePixelFormat ] = "yuv422p";
	dnxhd185[ "g" ] = "1";
	dnxhd185[ constants::mediaProfileFrameRate ] = "25";

	Profile::ProfileDesc dnxhd185x;
	dnxhd185x[ constants::mediaProfileIdentificator ] = "dnxhd185x";
	dnxhd185x[ constants::mediaProfileIdentificatorHuman ] = "DNxHD 185 X";
	dnxhd185x[ constants::mediaProfileType ] = constants::mediaProfileTypeVideo;
	dnxhd185x[ constants::mediaProfileCodec ] = "dnxhd";
	dnxhd185x[ "b" ] = "185000000";
	dnxhd185x[ constants::mediaProfilePixelFormat ] = "yuv422p10";
	dnxhd185x[ "g" ] = "1";
	dnxhd185x[ constants::mediaProfileFrameRate ] = "25";

	profiles.push_back( dnxhd120 );
	profiles.push_back( dnxhd185 );
	profiles.push_back( dnxhd185x );

}

}
}

#endif
