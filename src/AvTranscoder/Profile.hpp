#ifndef _AV_TRANSCODER_PROFILE_HPP_
#define _AV_TRANSCODER_PROFILE_HPP_

#include <string>
#include <cstring>
#include <vector>
#include <map>

namespace avtranscoder
{

class Profile
{
public:
	static const std::string avProfileIdentificator;
	static const std::string avProfileIdentificatorHuman;
	static const std::string avProfileType;

	static const std::string avProfileTypeFormat;
	static const std::string avProfileTypeVideo;
	static const std::string avProfileTypeAudio;
	
	static const std::string avProfileFormat;
	static const std::string avProfileCodec;
	static const std::string avProfilePixelFormat;
	static const std::string avProfileSampleFormat;
	static const std::string avProfileFrameRate;
	static const std::string avProfileSampleRate;
	static const std::string avProfileChannel;

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