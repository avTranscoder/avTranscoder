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
	const std::string avProfileIdentificator = "avProfileName";
	const std::string avProfileIdentificatorHuman = "avProfileLongName";
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

class AvExport ProfileLoader
{
public:
	typedef std::map< std::string, std::string > Profile;
	typedef std::vector< Profile > Profiles;

public:
	ProfileLoader( bool autoload = false );

	void loadProfiles( const std::string& avProfilesPath = "" );
	void loadProfile( const std::string& avProfileFile );

	void update( const Profile& profile );
	
	const Profiles& getProfiles();

	Profiles getFormatProfiles();
	Profiles getVideoProfiles();
	Profiles getAudioProfiles();

	Profile& getProfile( const std::string& searchProfile );

private:
	Profiles _profiles;
};

}
#endif
