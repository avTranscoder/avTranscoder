#ifndef _AVTRANSCODER_PROFIL_WAVE_HPP_
#define _AVTRANSCODER_PROFIL_WAVE_HPP_

namespace avtranscoder
{

void loadWave( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc wave16bits48kmono;

	wave16bits48kmono[ Profile::avProfileIdentificator ] = "wave16bits48kmono";
	wave16bits48kmono[ Profile::avProfileIdentificatorHuman ] = "Wave 16bits 48k mono";
	wave16bits48kmono[ Profile::avProfileType ] = Profile::avProfileTypeAudio;

	wave16bits48kmono[ "codec" ] = "pcm_s16le";
	wave16bits48kmono[ "sample_fmt" ] = "s16";
	wave16bits48kmono[ "channels" ] = "1";
	wave16bits48kmono[ "sample_rate" ] = "48000";

	Profile::ProfileDesc wave24bits48kmono;

	wave24bits48kmono[ Profile::avProfileIdentificator ] = "wave24bits48kmono";
	wave24bits48kmono[ Profile::avProfileIdentificatorHuman ] = "Wave 24bits 48k mono";
	wave24bits48kmono[ Profile::avProfileType ] = Profile::avProfileTypeAudio;

	wave24bits48kmono[ "codec" ] = "pcm_s24le";
	wave24bits48kmono[ "sample_fmt" ] = "s32";
	wave24bits48kmono[ "channels" ] = "1";
	wave24bits48kmono[ "sample_rate" ] = "48000";


	Profile::ProfileDesc wave16bits48kstereo;

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
