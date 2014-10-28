#include "Profile.hpp"

#include <mediaCore/profile/XdCamHd422.hpp>
#include <mediaCore/profile/DNxHD.hpp>
#include <mediaCore/profile/Wave.hpp>
#include <mediaCore/profile/Avi.hpp>
#include <mediaCore/profile/Mkv.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

namespace mediamanager
{
namespace mediacore
{

Profile::Profile( bool autoload )
{
	if( autoload )
		loadProfiles();
}

void Profile::loadProfile( const std::string& mediaProfileFile )
{
	std::ifstream infile;
	infile.open( mediaProfileFile.c_str(), std::ifstream::in );
	
	Profile::ProfileDesc customProfile;

	std::string line;
	while( std::getline( infile, line ) )
	{
		std::vector< std::string > keyValue;
		split( keyValue, line, "=" );
		if( keyValue.size() == 2 )
			customProfile[ keyValue.at( 0 ) ] = keyValue.at( 1 );
	}
	// check if profile contains required values
	if( 
		customProfile.count( constants::mediaProfileIdentificator ) &&
		customProfile.count( constants::mediaProfileIdentificatorHuman ) &&
		customProfile.count( constants::mediaProfileType ) &&
		( customProfile.find( constants::mediaProfileType )->second == constants::mediaProfileTypeVideo ||
		customProfile.find( constants::mediaProfileType )->second == constants::mediaProfileTypeAudio )
		)
	{
		_profiles.push_back( customProfile );
	}
}

void Profile::loadProfiles( const std::string& mediaProfilesPath )
{
	loadXdCamHD422( _profiles );
	loadDNxHD( _profiles );
	loadWave( _profiles );
	loadAvi( _profiles );
	loadMkv( _profiles );
	
	std::string realMediaProfilesPath = mediaProfilesPath;
	if( realMediaProfilesPath.empty() )
	{
		if( std::getenv( "MEDIAPROFILES" ) )
			realMediaProfilesPath = std::getenv( "MEDIA_PROFILES" );
		else
			return;
	}
	
	std::vector< std::string > paths;
	split( paths, realMediaProfilesPath, ":" );
	for( std::vector< std::string >::iterator dirIt = paths.begin(); dirIt != paths.end(); ++dirIt )
	{
		std::vector< std::string > files;
		if( getFilesInDir( *dirIt, files ) != 0 )
			continue;

		for( std::vector< std::string >::iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt )
		{
			const std::string absPath = ( *dirIt ) + "/" + ( *fileIt );
			loadProfile( absPath );
		}
	}
}

void Profile::update( const ProfileDesc& profile )
{
	std::string profileId( profile.find( constants::mediaProfileIdentificator )->second );
	size_t profileIndex = 0;
	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::mediaProfileIdentificator )->second == profileId )
		{
			_profiles.at( profileIndex ) = profile;
			return;
		}
		++profileIndex;
	}
	// profile not found: add the new profile
	_profiles.push_back( profile );
}

const Profile::ProfilesDesc& Profile::getProfiles()
{
	return _profiles;
}

Profile::ProfilesDesc Profile::getFormatProfiles()
{
	ProfilesDesc profiles;

	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::mediaProfileType )->second == constants::mediaProfileTypeFormat )
			profiles.push_back( *it );
	}

	return profiles;
}

Profile::ProfilesDesc Profile::getVideoProfiles()
{
	ProfilesDesc profiles;

	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::mediaProfileType )->second == constants::mediaProfileTypeVideo )
			profiles.push_back( *it );
	}

	return profiles;
}

Profile::ProfilesDesc Profile::getAudioProfiles()
{
	ProfilesDesc profiles;

	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::mediaProfileType )->second == constants::mediaProfileTypeAudio )
			profiles.push_back( *it );
	}

	return profiles;
}

Profile::ProfileDesc& Profile::getProfile( const std::string& searchProfile )
{
	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::mediaProfileIdentificator )->second == searchProfile )
		{
			return (*it);
		}
	}
	throw std::runtime_error( "unable to find profile: " + searchProfile );
}

}
}
