#ifndef _AVTRANSCODER_PROFIL_WAVE_HPP_
#define _AVTRANSCODER_PROFIL_WAVE_HPP_

namespace avtranscoder
{

void loadWave( ProfileLoader::Profiles& profiles )
{
	ProfileLoader::Profile wave48k;
	wave48k[ constants::avProfileType ] = constants::avProfileTypeAudio;
	wave48k[ constants::avProfileSampleRate ] = "48000";

	// 48kHz - 16bits little endian - mono
	wave48k[ constants::avProfileIdentificator ] = "wave16b48kmono";
	wave48k[ constants::avProfileIdentificatorHuman ] = "Wave 16bits 48kHz mono";
	wave48k[ constants::avProfileCodec ] = "pcm_s16le";
	wave48k[ constants::avProfileSampleFormat ] = "s16";
	wave48k[ constants::avProfileChannel ] = "1";

	profiles.push_back( wave48k );

	// 48kHz - 16bits little endian - stereo
	wave48k[ constants::avProfileIdentificator ] = "wave16b48kstereo";
	wave48k[ constants::avProfileIdentificatorHuman ] = "Wave 16bits 48kHz stereo";
	wave48k[ constants::avProfileChannel ] = "2";

	profiles.push_back( wave48k );

	// 48kHz - 16bits little endian - 5.1
	wave48k[ constants::avProfileIdentificator ] = "wave16b48k5_1";
	wave48k[ constants::avProfileIdentificatorHuman ] = "Wave 16bits 48kHz 5.1";
	wave48k[ constants::avProfileChannel ] = "6";

	profiles.push_back( wave48k );

	// 48kHz - 24bits little endian - mono
	wave48k[ constants::avProfileIdentificator ] = "wave24b48kmono";
	wave48k[ constants::avProfileIdentificatorHuman ] = "Wave 24bits 48kHz mono";
	wave48k[ constants::avProfileCodec ] = "pcm_s24le";
	wave48k[ constants::avProfileSampleFormat ] = "s32";
	wave48k[ constants::avProfileChannel ] = "1";

	profiles.push_back( wave48k );

	// 48kHz - 24bits little endian - stereo
	wave48k[ constants::avProfileIdentificator ] = "wave24b48kstereo";
	wave48k[ constants::avProfileIdentificatorHuman ] = "Wave 24bits 48kHz stereo";
	wave48k[ constants::avProfileChannel ] = "2";

	profiles.push_back( wave48k );

	// 48kHz - 24bits little endian - 5.1
	wave48k[ constants::avProfileIdentificator ] = "wave24b48k5_1";
	wave48k[ constants::avProfileIdentificatorHuman ] = "Wave 24bits 48kHz 5.1";
	wave48k[ constants::avProfileChannel ] = "6";

	profiles.push_back( wave48k );
}

}

#endif
