#ifndef _AV_TRANSCODER_PROFILE_HPP_
#define _AV_TRANSCODER_PROFILE_HPP_

#include <AvTranscoder/common.hpp>

#include <string>
#include <vector>
#include <map>
#include <iostream>

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
	const std::string avProfileThreads = "threads";
	const std::string avProfileProcessStat = "processStat";  ///< Do statistics during the process.
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
	 * @brief Load the given profile
	 * @exception throw std::runtime_error if the profile is invalid
	 */
	void loadProfile( const Profile& profile );

	bool hasProfile( const Profile& profile ) const;

	const Profiles& getProfiles() const;

	Profiles getFormatProfiles() const;
	Profiles getVideoProfiles() const;
	Profiles getAudioProfiles() const;

	const Profile& getProfile( const std::string& avProfileIdentificator ) const;

public:
	static bool checkFormatProfile( const Profile& profileToCheck );
	static bool checkVideoProfile( const Profile& profileToCheck );
	static bool checkAudioProfile( const Profile& profileToCheck );

private:
	Profiles _profiles;
};

#ifndef SWIG
// To print a profile
std::ostream &operator<<( std::ostream &os, const ProfileLoader::Profile &profile );
#endif

}
#endif
