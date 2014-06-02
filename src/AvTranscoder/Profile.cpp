#include "Profile.hpp"

#include <AvTranscoder/Profiles/XdCamHd422.hpp>
#include <AvTranscoder/Profiles/DNxHD.hpp>

namespace avtranscoder
{

const std::string Profile::avProfilIdentificator( "avProfile" );
const std::string Profile::avProfilIdentificatorHuman( "avProfileLong" );


Profile::Profile()
{

}

void Profile::loadProfiles()
{
	loadXdCamHD422( _profiles );
	loadDNxHD( _profiles );
}

const Profile::ProfilesDesc& Profile::getProfiles()
{
	return _profiles;
}

Profile::ProfilesDesc Profile::getVideoProfiles()
{
	ProfilesDesc profiles;

	return profiles;
}

Profile::ProfilesDesc Profile::getAudioProfiles()
{
	ProfilesDesc profiles;

	return profiles;
}

Profile::ProfileDesc& Profile::getProfile( const std::string& searchProfile )
{
	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it)["avProfile"] == searchProfile )
		{
			return (*it);
		}
	}
}

/*
ffmpeg can set options with 
int av_opt_set(void *obj, const char *name, const char *val, int search_flags)

can be string / string parameter, ffmpeg can detect option type and convert value to match

*/

}
