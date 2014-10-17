#ifndef _AV_TRANSCODER_PROFILE_HPP_
#define _AV_TRANSCODER_PROFILE_HPP_

#include "common.hpp"

#include <string>
#include <cstring>
#include <vector>
#include <map>

namespace avtranscoder
{

namespace constants
{
	const std::string avProfileIdentificator = "avProfile";
	const std::string avProfileIdentificatorHuman = "avProfileLong";
	const std::string avProfileType = "avProfileType";
	const std::string avProfileTypeFormat = "avProfileTypeFormat";
	const std::string avProfileTypeVideo = "avProfileTypeVideo";
	const std::string avProfileTypeAudio = "avProfileTypeAudio";
	const std::string avProfileFormat = "format";
	const std::string avProfileCodec = "codec";
	const std::string avProfilePixelFormat = "pix_fmt";
	const std::string avProfileSampleFormat = "sample_fmt";
	const std::string avProfileFrameRate = "r";
	const std::string avProfileSampleRate = "ar";
	const std::string avProfileChannel = "ac";
}

class AvExport Profile
{
public:


public:
	// typedef std::pair< std::string, std::string > KeyDesc;
	typedef std::map< std::string, std::string > ProfileDesc;
	typedef std::vector< ProfileDesc > ProfilesDesc;

public:
	Profile( bool autoload = false );

	void loadProfiles( const std::string& avProfilesPath = "" );
	void loadProfile( const std::string& avProfileFile );

	void update( const ProfileDesc& profile );
	
	const ProfilesDesc& getProfiles();

	ProfilesDesc getFormatProfiles();
	ProfilesDesc getVideoProfiles();
	ProfilesDesc getAudioProfiles();

	ProfileDesc& getProfile( const std::string& searchProfile );

private:
	ProfilesDesc _profiles;

};



}
#endif