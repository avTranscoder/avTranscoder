#include "ProfileLoader.hpp"

#include "common.hpp"

#include <AvTranscoder/profile/XdCamHd422.hpp>
#include <AvTranscoder/profile/DNxHD.hpp>
#include <AvTranscoder/profile/Wave.hpp>
#include <AvTranscoder/profile/Avi.hpp>
#include <AvTranscoder/profile/Mkv.hpp>

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
	// check if profile contains required values
	if( 
		customProfile.count( constants::avProfileIdentificator ) &&
		customProfile.count( constants::avProfileIdentificatorHuman ) &&
		customProfile.count( constants::avProfileType ) &&
		( customProfile.find( constants::avProfileType )->second == constants::avProfileTypeFormat ||
		customProfile.find( constants::avProfileType )->second == constants::avProfileTypeVideo ||
		customProfile.find( constants::avProfileType )->second == constants::avProfileTypeAudio )
		)
	{
		_profiles.push_back( customProfile );
	}
}

void ProfileLoader::loadProfiles( const std::string& avProfilesPath )
{
	loadXdCamHD422( _profiles );
	loadDNxHD( _profiles );
	loadWave( _profiles );
	loadAvi( _profiles );
	loadMkv( _profiles );
	
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

ProfileLoader::Profile& ProfileLoader::getProfile( const std::string& searchProfile )
{
	for( Profiles::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( constants::avProfileIdentificator )->second == searchProfile )
		{
			return (*it);
		}
	}
	throw std::runtime_error( "unable to find profile: " + searchProfile );
}

}
