#ifndef _MEDIA_ENGINE_CORE_PROFILE_PROFILE_HPP_
#define _MEDIA_ENGINE_CORE_PROFILE_PROFILE_HPP_

#include <mediaCore/common.hpp>

#include <string>
#include <cstring>
#include <vector>
#include <map>

namespace mediaengine
{
namespace mediacore
{

namespace constants
{
	const std::string mediaProfileIdentificator = "mediaProfile";
	const std::string mediaProfileIdentificatorHuman = "mediaProfileLong";
	const std::string mediaProfileType = "mediaProfileType";
	const std::string mediaProfileTypeFormat = "mediaProfileTypeFormat";
	const std::string mediaProfileTypeVideo = "mediaProfileTypeVideo";
	const std::string mediaProfileTypeAudio = "mediaProfileTypeAudio";
	const std::string mediaProfileFormat = "format";
	const std::string mediaProfileCodec = "codec";
	const std::string mediaProfilePixelFormat = "pix_fmt";
	const std::string mediaProfileSampleFormat = "sample_fmt";
	const std::string mediaProfileFrameRate = "r";
	const std::string mediaProfileSampleRate = "ar";
	const std::string mediaProfileChannel = "ac";
}

class MediaEngineExport Profile
{
public:
	typedef std::map< std::string, std::string > ProfileDesc;
	typedef std::vector< ProfileDesc > ProfilesDesc;

public:
	Profile( bool autoload = false );

	void loadProfiles( const std::string& mediaProfilesPath = "" );
	void loadProfile( const std::string& mediaProfileFile );

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
}

#endif
