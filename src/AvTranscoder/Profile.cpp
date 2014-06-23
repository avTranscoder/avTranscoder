#include "Profile.hpp"

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/Profiles/XdCamHd422.hpp>
#include <AvTranscoder/Profiles/DNxHD.hpp>
#include <AvTranscoder/Profiles/Wave.hpp>

#include <iostream>
#include <cstdlib>

namespace avtranscoder
{

const std::string Profile::avProfileIdentificator( "avProfile" );
const std::string Profile::avProfileIdentificatorHuman( "avProfileLong" );
const std::string Profile::avProfileType( "avProfileType" );
const std::string Profile::avProfileTypeVideo( "avProfileTypeVideo" );
const std::string Profile::avProfileTypeAudio( "avProfileTypeAudio" );


Profile::Profile()
{

}

void Profile::loadProfiles()
{
	loadXdCamHD422( _profiles );
	loadDNxHD( _profiles );
	loadWave( _profiles );

	if( const char* envAvProfiles = std::getenv("AVPROFILES") )
	{
		std::vector< std::string > paths;
		split( paths, envAvProfiles, ":" );
		for( std::vector< std::string >::iterator it = paths.begin(); it != paths.end(); ++it )
		{
			std::cout << "search profile in path " << *it << std::endl;
		}
	}
}

const Profile::ProfilesDesc& Profile::getProfiles()
{
	return _profiles;
}

Profile::ProfilesDesc Profile::getVideoProfiles()
{
	ProfilesDesc profiles;

	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( avProfileType )->second == avProfileTypeVideo )
			profiles.push_back( *it );
	}

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
		if( (*it).find( avProfileIdentificator )->second == searchProfile )
		{
			return (*it);
		}
	}
}

}
