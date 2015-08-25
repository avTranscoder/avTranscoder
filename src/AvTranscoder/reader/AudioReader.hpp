#ifndef _AV_TRANSCODER_AUDIOREADER_HPP
#define _AV_TRANSCODER_AUDIOREADER_HPP

#include "IReader.hpp"

#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/mediaProperty/AudioProperties.hpp>
#include <AvTranscoder/decoder/AudioDecoder.hpp>
#include <AvTranscoder/frame/AudioFrame.hpp>
#include <AvTranscoder/transform/AudioTransform.hpp>

namespace avtranscoder
{

class AvExport AudioReader : public IReader
{
public:
	/**
	 * @brief Create a new InputFile and prepare to read the audio stream at the given index
	 */
	AudioReader( const std::string& filename, const size_t audioStreamIndex );

	/**
	 * @brief Get the existing InputFile and prepare to read the audio stream at the given index
	 * @note This constructor can improve performances when you create several AudioReader from one InputFile.
	 */
	AudioReader( InputFile& inputFile, const size_t audioStreamIndex );

	~AudioReader();

	size_t getSampleRate();
	size_t getChannels();

	const char* readNextFrame();
	const char* readPrevFrame();
	const char* readFrameAt( const size_t frame );

	void printInfo();

private:
	void init();

private:
	InputFile* _inputFile;
	const AudioProperties* _audioProperties;
	AudioDecoder* _audioDecoder;

	AudioFrame* _sourceFrame;
	AudioFrame* _dstFrame;

	AudioTransform _audioTransform;

	size_t _audioStreamIndex;

	bool _inputFileAllocated;
};

}

#endif
