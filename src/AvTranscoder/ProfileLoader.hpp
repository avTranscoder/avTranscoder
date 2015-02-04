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
	const std::string avProfileWidth = "width";
	const std::string avProfileHeight = "height";
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

	/**
	 * @brief Load profiles from files in avProfilesPath directory
	 * @param avProfilesPath: if empty, the path is replaced by value of AVPROFILES environment variable
	 */
	void loadProfiles( const std::string& avProfilesPath = "" );

	/**
	 * @brief Load the profile defines in the given file
	 */
	void loadProfile( const std::string& avProfileFileName );

	/**
	 * @brief  Load the given profile
	 * @exception throw std::runtime_error if the profile is invalid
	 */
	void loadProfile( const Profile& profile );

	const Profiles& getProfiles();

	Profiles getFormatProfiles();
	Profiles getVideoProfiles();
	Profiles getAudioProfiles();

	Profile& getProfile( const std::string& avProfileIdentificator );

private:
	bool checkFormatProfile( const Profile& profileToCheck );
	bool checkVideoProfile( const Profile& profileToCheck );
	bool checkAudioProfile( const Profile& profileToCheck );

private:
	Profiles _profiles;
};

#ifndef SWIG
void split( std::vector< std::string >& splitString, const std::string& inputString, const std::string& splitChars = ";" );
int getFilesInDir( const std::string& dir, std::vector< std::string >& files );
#endif

}
#endif
