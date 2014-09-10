#ifndef _AVTRANSCODER_PROFIL_WAVE_HPP_
#define _AVTRANSCODER_PROFIL_WAVE_HPP_

namespace avtranscoder
{

void loadWave( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc wave24b48kMono;

	wave24b48kMono[ Profile::avProfileIdentificator ] = "wave24b48kmono";
	wave24b48kMono[ Profile::avProfileIdentificatorHuman ] = "Wave 24bits 48kHz mono";
	wave24b48kMono[ Profile::avProfileType ] = Profile::avProfileTypeAudio;

	wave24b48kMono[ Profile::avProfileCodec ] = "pcm_s24le";
	wave24b48kMono[ Profile::avProfileSampleFormat ] = "s32";
	wave24b48kMono[ Profile::avProfileSampleRate ] = "48000";
	wave24b48kMono[ Profile::avProfileChannel ] = "1";

	Profile::ProfileDesc wave16b48kMono;

	wave16b48kMono[ Profile::avProfileIdentificator ] = "wave16b48kmono";
	wave16b48kMono[ Profile::avProfileIdentificatorHuman ] = "Wave 16bits 48kHz mono";
	wave16b48kMono[ Profile::avProfileType ] = Profile::avProfileTypeAudio;

	wave16b48kMono[ Profile::avProfileCodec ] = "pcm_s16le";
	wave16b48kMono[ Profile::avProfileSampleFormat ] = "s16";
	wave16b48kMono[ Profile::avProfileSampleRate ] = "48000";
	wave16b48kMono[ Profile::avProfileChannel ] = "1";

	profiles.push_back( wave24b48kMono );
	profiles.push_back( wave16b48kMono );

	wave16bits48kstereo[ Profile::avProfileIdentificator ] = "wave16bits48kstereo";
	wave16bits48kstereo[ Profile::avProfileIdentificatorHuman ] = "Wave 16bits 48k stereo";
	wave16bits48kstereo[ Profile::avProfileType ] = Profile::avProfileTypeAudio;

	wave16bits48kstereo[ "codec" ] = "pcm_s16le";
	wave16bits48kstereo[ "sample_fmt" ] = "s16";
	wave16bits48kstereo[ "channels" ] = "2";
	wave16bits48kstereo[ "sample_rate" ] = "48000";

	Profile::ProfileDesc wave24bits48kstereo;

	wave24bits48kstereo[ Profile::avProfileIdentificator ] = "wave24bits48kstereo";
	wave24bits48kstereo[ Profile::avProfileIdentificatorHuman ] = "Wave 24bits 48k stereo";
	wave24bits48kstereo[ Profile::avProfileType ] = Profile::avProfileTypeAudio;

	wave24bits48kstereo[ "codec" ] = "pcm_s24le";
	wave24bits48kstereo[ "sample_fmt" ] = "s32";
	wave24bits48kstereo[ "channels" ] = "2";
	wave24bits48kstereo[ "sample_rate" ] = "48000";


	profiles.push_back( wave16bits48kmono );
	profiles.push_back( wave24bits48kmono );
	profiles.push_back( wave16bits48kstereo );
	profiles.push_back( wave24bits48kstereo );
}

}

#endif
