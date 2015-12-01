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
	//@{
	// @note Transform the input stream to s16 sample format (to listen).
	// @see updateOutput
	AudioReader( const std::string& filename, const size_t streamIndex = 0, const int channelIndex = -1 );
	AudioReader( InputFile& inputFile, const size_t streamIndex = 0, const int channelIndex = -1 );
	//@}

	~AudioReader();

	/**
	 * @brief Update sample rate, number of channels and sample format of the output.
	 * @note Will transform the decoded data when read the stream.
	 */
	void updateOutput( const size_t sampleRate, const size_t nbChannels, const std::string& sampleFormat );

	//@{
	// @brief Output info
	size_t getOutputSampleRate() const { return _outputSampleRate; }
	size_t getOutputChannels() const { return _outputNbChannels; }
	AVSampleFormat getOutputSampleFormat() const { return _outputSampleFormat; }
	//@}

	// @brief Input info
	const AudioProperties* getAudioProperties() const {return _audioStreamProperties;}

	void printInfo();

private:
	void init();

private:
	const AudioProperties* _audioStreamProperties;  ///< Properties of the source audio stream read (no ownership, has link)

	//@{
	// @brief Output info
	size_t _outputSampleRate;
	size_t _outputNbChannels;
	AVSampleFormat _outputSampleFormat;
	//@}
};

}

#endif
