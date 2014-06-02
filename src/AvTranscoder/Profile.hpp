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
	static const std::string avProfilIdentificator;
	static const std::string avProfilIdentificatorHuman;

	// typedef std::pair< std::string, std::string > KeyDesc;
	typedef std::map< std::string, std::string > ProfileDesc;
	typedef std::vector< ProfileDesc > ProfilesDesc;

	Profile();

	void loadProfiles();

	const ProfilesDesc& getProfiles();

	ProfilesDesc getVideoProfiles();
	ProfilesDesc getAudioProfiles();

	ProfileDesc& getProfile( const std::string& searchProfile );

private:

	ProfilesDesc _profiles;
};

}
#endif