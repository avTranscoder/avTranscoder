#ifndef _AVTRANSCODER_PROFIL_WAVE_HPP_
#define _AVTRANSCODER_PROFIL_WAVE_HPP_

namespace avtranscoder
{

void loadWave( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc wave24b48kMono;

	wave24b48kMono[ Profile::avProfileIdentificator ] = "wave24b48kMono";
	wave24b48kMono[ Profile::avProfileIdentificatorHuman ] = "Wave 24bits 48kHz mono";
	wave24b48kMono[ Profile::avProfileType ] = Profile::avProfileTypeAudio;

	wave24b48kMono[ Profile::avProfileCodec ] = "pcm_s24le";
	wave24b48kMono[ Profile::avProfileSampleFormat ] = "s32";
	wave24b48kMono[ Profile::avProfileSampleRate ] = "48000";
	wave24b48kMono[ Profile::avProfileChannel ] = "1";
	wave24b48kMono[ Profile::avProfileChannelLayout ] = "1";

	Profile::ProfileDesc wave16b48kMono;

	wave16b48kMono[ Profile::avProfileIdentificator ] = "wave16b48kMono";
	wave16b48kMono[ Profile::avProfileIdentificatorHuman ] = "Wave 16bits 48kHz mono";
	wave16b48kMono[ Profile::avProfileType ] = Profile::avProfileTypeAudio;

	wave16b48kMono[ Profile::avProfileCodec ] = "pcm_s16le";
	wave16b48kMono[ Profile::avProfileSampleFormat ] = "s16";
	wave16b48kMono[ Profile::avProfileSampleRate ] = "48000";
	wave16b48kMono[ Profile::avProfileChannel ] = "1";
	wave16b48kMono[ Profile::avProfileChannelLayout ] = "1";

	profiles.push_back( wave24b48kMono );
	profiles.push_back( wave16b48kMono );
}

}

#endif
