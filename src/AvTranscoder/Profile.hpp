#ifndef _AV_TRANSCODER_PROFILE_HPP_
#define _AV_TRANSCODER_PROFILE_HPP_

#include "common.hpp"

#include <string>
#include <cstring>
#include <vector>
#include <map>

namespace avtranscoder
{

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

	static const std::string & getAvProfileIdentificatorKey();
	static const std::string & getAvProfileIdentificatorHumanKey();
	static const std::string & getAvProfileTypeKey();
	static const std::string & getAvProfileTypeFormatKey();
	static const std::string & getAvProfileTypeVideoKey();
	static const std::string & getAvProfileTypeAudioKey();
	static const std::string & getAvProfileFormatKey();
	static const std::string & getAvProfileCodecKey();
	static const std::string & getAvProfilePixelFormatKey();
	static const std::string & getAvProfileSampleFormatKey();
	static const std::string & getAvProfileFrameRateKey();
	static const std::string & getAvProfileSampleRateKey();
	static const std::string & getAvProfileChannelKey();

private:
	ProfilesDesc _profiles;

#ifndef SWIG
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
#endif

};



}
#endif