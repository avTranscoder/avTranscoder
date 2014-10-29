#ifndef _MEDIA_ENGINE_CORE_PROFILE_WAVE_HPP_
#define _MEDIA_ENGINE_CORE_PROFILE_WAVE_HPP_

namespace mediaengine
{
namespace mediacore
{

void loadWave( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc wave48k;
	wave48k[ constants::mediaProfileType ] = constants::mediaProfileTypeAudio;
	wave48k[ constants::mediaProfileSampleRate ] = "48000";

	// 48kHz - 16bits little endian - mono
	wave48k[ constants::mediaProfileIdentificator ] = "wave16b48kmono";
	wave48k[ constants::mediaProfileIdentificatorHuman ] = "Wave 16bits 48kHz mono";
	wave48k[ constants::mediaProfileCodec ] = "pcm_s16le";
	wave48k[ constants::mediaProfileSampleFormat ] = "s16";
	wave48k[ constants::mediaProfileChannel ] = "1";

	profiles.push_back( wave48k );

	// 48kHz - 16bits little endian - stereo
	wave48k[ constants::mediaProfileIdentificator ] = "wave16b48kstereo";
	wave48k[ constants::mediaProfileIdentificatorHuman ] = "Wave 16bits 48kHz stereo";
	wave48k[ constants::mediaProfileChannel ] = "2";

	profiles.push_back( wave48k );

	// 48kHz - 16bits little endian - 5.1
	wave48k[ constants::mediaProfileIdentificator ] = "wave16b48k5_1";
	wave48k[ constants::mediaProfileIdentificatorHuman ] = "Wave 16bits 48kHz 5.1";
	wave48k[ constants::mediaProfileChannel ] = "6";

	profiles.push_back( wave48k );

	// 48kHz - 24bits little endian - mono
	wave48k[ constants::mediaProfileIdentificator ] = "wave24b48kmono";
	wave48k[ constants::mediaProfileIdentificatorHuman ] = "Wave 24bits 48kHz mono";
	wave48k[ constants::mediaProfileCodec ] = "pcm_s24le";
	wave48k[ constants::mediaProfileSampleFormat ] = "s32";
	wave48k[ constants::mediaProfileChannel ] = "1";

	profiles.push_back( wave48k );

	// 48kHz - 24bits little endian - stereo
	wave48k[ constants::mediaProfileIdentificator ] = "wave24b48kstereo";
	wave48k[ constants::mediaProfileIdentificatorHuman ] = "Wave 24bits 48kHz stereo";
	wave48k[ constants::mediaProfileChannel ] = "2";

	profiles.push_back( wave48k );

	// 48kHz - 24bits little endian - 5.1
	wave48k[ constants::mediaProfileIdentificator ] = "wave24b48k5_1";
	wave48k[ constants::mediaProfileIdentificatorHuman ] = "Wave 24bits 48kHz 5.1";
	wave48k[ constants::mediaProfileChannel ] = "6";

	profiles.push_back( wave48k );
}

}
}

#endif
