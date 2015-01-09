#include "ProfileLoader.hpp"

#include "common.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

namespace avtranscoder
{

ProfileLoader::ProfileLoader( bool autoload )
{
	if( autoload )
		loadProfiles();
}

void ProfileLoader::loadProfile( const std::string& avProfileFile )
{
	std::ifstream infile;
	infile.open( avProfileFile.c_str(), std::ifstream::in );
	
	ProfileLoader::Profile customProfile;

	std::string line;
	while( std::getline( infile, line ) )
	{
		std::vector< std::string > keyValue;
		split( keyValue, line, "=" );
		if( keyValue.size() == 2 )
			customProfile[ keyValue.at( 0 ) ] = keyValue.at( 1 );
	}

	// check profile long name
	if( ! customProfile.count( constants::avProfileIdentificator ) )
	{
		std::cout << "Warning: A profile has no name. It will not be loaded." << std::endl;
		return;
	}

	// check profile type
	if( customProfile.count( constants::avProfileType ) == 0 )
	{
		std::cout << "Warning: The profile " << customProfile.find( constants::avProfileIdentificator )->second << " has not type. It will not be loaded." << std::endl;
		return;
	}

	// check complete profile
	bool isValid = false;
	std::string type( customProfile.find( constants::avProfileType )->second );
	if( type == constants::avProfileTypeFormat )
		isValid = checkFormatProfile( customProfile );
	else if( type == constants::avProfileTypeVideo )
		isValid = checkVideoProfile( customProfile );
	else if( type == constants::avProfileTypeAudio )
		isValid = checkAudioProfile( customProfile );

	if( isValid )
		_profiles.push_back( customProfile );
	else
		std::cout << "Warning: The profile " << customProfile.find( constants::avProfileIdentificator )->second << " is invalid. It will not be loaded." << std::endl;
}

void ProfileLoader::loadProfiles( const std::string& avProfilesPath )
{	
	std::string realAvProfilesPath = avProfilesPath;
	if( realAvProfilesPath.empty() )
	{
		if( std::getenv( "AVPROFILES" ) )
			realAvProfilesPath = std::getenv( "AVPROFILES" );
		else
			return;
	}
	
	std::vector< std::string > paths;
	split( paths, realAvProfilesPath, ":" );
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

void ProfileLoader::update( const Profile& profile )
{
	Profile::const_iterator profileIt = profile.find( constants::avProfileIdentificator );
	if( profileIt == profile.end() )
		throw std::runtime_error( "Invalid profile: can't get identificator" );

	std::string profileId( profileIt->second );
	size_t profileIndex = 0;
	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		// profile already exists
		if( (*it).find( constants::avProfileIdentificator )->second == profileId )
			return;
		++profileIndex;
	}
	// profile not found: add the new profile
	_profiles.push_back( profile );
}

const ProfileLoader::Profiles& ProfileLoader::getProfiles()
{
	return _profiles;
}

ProfileLoader::Profiles ProfileLoader::getFormatProfiles()
{
	Profiles profiles;

	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileType )->second == constants::avProfileTypeFormat )
			profiles.push_back( *it );
	}

	return profiles;
}

ProfileLoader::Profiles ProfileLoader::getVideoProfiles()
{
	Profiles profiles;

	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileType )->second == constants::avProfileTypeVideo )
			profiles.push_back( *it );
	}

	return profiles;
}

ProfileLoader::Profiles ProfileLoader::getAudioProfiles()
{
	Profiles profiles;

	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileType )->second == constants::avProfileTypeAudio )
			profiles.push_back( *it );
	}

	return profiles;
}

ProfileLoader::Profile& ProfileLoader::getProfile( const std::string& avProfileIdentificator )
{
	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileIdentificator )->second == avProfileIdentificator )
		{
			return (*it);
		}
	}
	throw std::runtime_error( "unable to find profile: " + avProfileIdentificator );
}


bool ProfileLoader::checkFormatProfile( const Profile& profileToCheck )
{
	bool isValid = true;

	if( ! profileToCheck.count( constants::avProfileIdentificator ) ||
		! profileToCheck.count( constants::avProfileIdentificatorHuman ) ||
		! profileToCheck.count( constants::avProfileType ) ||
		! profileToCheck.count( constants::avProfileFormat ) )
	{
		isValid = false;
	}

	return isValid;
}

bool ProfileLoader::checkVideoProfile( const Profile& profileToCheck )
{
	bool isValid = true;

	if( ! profileToCheck.count( constants::avProfileIdentificator ) ||
		! profileToCheck.count( constants::avProfileIdentificatorHuman ) ||
		! profileToCheck.count( constants::avProfileType ) ||
		! profileToCheck.count( constants::avProfileCodec ) ||
		! profileToCheck.count( constants::avProfileFrameRate ) )
	{
		isValid = false;
	}

	return isValid;
}

bool ProfileLoader::checkAudioProfile( const Profile& profileToCheck )
{
	bool isValid = true;

	if( ! profileToCheck.count( constants::avProfileIdentificator ) ||
		! profileToCheck.count( constants::avProfileIdentificatorHuman ) ||
		! profileToCheck.count( constants::avProfileType ) ||
		! profileToCheck.count( constants::avProfileCodec ) )
	{
		isValid = false;
	}

	return isValid;
}


}
