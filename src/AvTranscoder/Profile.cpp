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
		for( std::vector< std::string >::iterator dirIt = paths.begin(); dirIt != paths.end(); ++dirIt )
		{
			//std::cout << "search profile in path " << *dirIt << std::endl;
			std::vector< std::string > files;
			if( getFilesInDir( *dirIt, files ) != 0 )
				continue;

			for( std::vector< std::string >::iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt )
			{
				if( ( *fileIt == "." ) || ( *fileIt == ".." ) )
					continue;

				std::ifstream ifs;

				ifs.open( ( *dirIt ) + ( *fileIt ), std::ifstream::in );

				std::cout << "file " << *dirIt << *fileIt << std::endl;
			}
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
