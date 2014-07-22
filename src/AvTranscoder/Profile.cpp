#include "Profile.hpp"

#include "common.hpp"

#include <AvTranscoder/Profiles/XdCamHd422.hpp>
#include <AvTranscoder/Profiles/DNxHD.hpp>
#include <AvTranscoder/Profiles/Wave.hpp>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>

namespace avtranscoder
{

const std::string Profile::avProfileIdentificator( "avProfile" );
const std::string Profile::avProfileIdentificatorHuman( "avProfileLong" );
const std::string Profile::avProfileType( "avProfileType" );
const std::string Profile::avProfileTypeVideo( "avProfileTypeVideo" );
const std::string Profile::avProfileTypeAudio( "avProfileTypeAudio" );
const std::string Profile::avProfileCodec( "codec" );
const std::string Profile::avProfilePixelFormat( "pix_fmt" );
const std::string Profile::avProfileSampleFormat( "sample_fmt" );
const std::string Profile::avProfileFrameRate( "r" );
const std::string Profile::avProfileSampleRate( "ar" );
const std::string Profile::avProfileChannel( "ac" );

Profile::Profile( bool autoload )
{
	if( autoload )
		loadProfiles();
}

void Profile::loadProfile( const std::string& avProfileFile )
{
	std::ifstream infile;
	infile.open( avProfileFile.c_str(), std::ifstream::in );
	
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
		customProfile.count( avProfileIdentificator ) &&
		customProfile.count( avProfileIdentificatorHuman ) &&
		customProfile.count( avProfileType ) &&
		( customProfile.find( avProfileType )->second == avProfileTypeVideo ||
		customProfile.find( avProfileType )->second == avProfileTypeAudio )
		)
	{
		_profiles.push_back( customProfile );
	}
}

void Profile::loadProfiles( const std::string& avProfilesPath )
{
	loadXdCamHD422( _profiles );
	loadDNxHD( _profiles );
	loadWave( _profiles );
	
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

void Profile::update( const ProfileDesc& profile )
{
	std::string profileId( profile.find( avProfileIdentificator )->second );
	size_t profileIndex = 0;
	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( avProfileIdentificator )->second == profileId )
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

	for( ProfilesDesc::iterator it = _profiles.begin(); it != _profiles.end(); ++it )
	{
		if( (*it).find( avProfileType )->second == avProfileTypeAudio )
			profiles.push_back( *it );
	}

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
	throw std::runtime_error( "unable to find profile: " + searchProfile );
}

}
