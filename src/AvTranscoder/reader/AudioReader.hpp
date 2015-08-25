#ifndef _AV_TRANSCODER_AUDIOREADER_HPP
#define _AV_TRANSCODER_AUDIOREADER_HPP

#include "IReader.hpp"

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>

namespace avtranscoder
{

class AvExport AudioReader : public IReader
{
public:
	AudioReader( const std::string& filename, const size_t audioStreamIndex );
	AudioReader( InputFile& inputFile, const size_t audioStreamIndex );

	~AudioReader();

	size_t getSampleRate();
	size_t getChannels();
	const AudioProperties* getAudioProperties() const {return _audioStreamProperties;}

	void printInfo();

private:
	void init();

private:
	const AudioProperties* _audioStreamProperties;  ///< Properties of the audio stream read (no ownership, has link)
};

}

#endif
