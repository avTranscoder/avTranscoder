#ifndef _MEDIA_ENGINE_IO_TRANSCODER_STREAM_TRANSCODER_HPP_
#define _MEDIA_ENGINE_IO_TRANSCODER_STREAM_TRANSCODER_HPP_

#include <mediaCore/common.hpp>
#include <mediaCore/profile/Profile.hpp>

#include <mediaIO/codedStream/IInputStream.hpp>
#include <mediaIO/codedStream/IOutputStream.hpp>
#include <mediaIO/essenceStream/AvInputVideo.hpp>
#include <mediaIO/essenceStream/AvOutputVideo.hpp>
#include <mediaIO/file/OutputFile.hpp>

namespace mediaengine
{

namespace mediatransform
{
	class ITransform;
}

namespace mediaio
{

class MediaEngineExport StreamTranscoder
{
public:
	/**
	 * @brief rewrap stream
	 * @note offset feature when rewrap a stream is not supported
	 **/
	StreamTranscoder( IInputStream& inputStream, OutputFile& outputFile );

	/**
	 * @brief transcode stream
	 **/
	StreamTranscoder( IInputStream& inputStream, OutputFile& outputFile, const mediacore::Profile::ProfileDesc& profile, const int subStreamIndex = -1, const size_t offset = 0 );

	/**
	 * @brief encode from a generated stream
	 * @note offset feature has no sense here
	 **/
	StreamTranscoder( const ICodec& inputCodec, OutputFile& outputFile, const mediacore::Profile::ProfileDesc& profile );

	~StreamTranscoder();

	/**
	 * @brief Init before encoding to pre-process frames necessary to delete the latency.
	 * @note This can be called several times with no side effects.
	 * @note Can take a little bit of time.
	 */
	void init();
	
	/**
	 * @brief process a single frame for the current stream
	 * @return the process status result
	 */
	bool processFrame();

	void switchEssence( bool swithToGenerator = true );
	void switchToGeneratorEssence();
	void switchToInputEssence();

	void setVerbose( bool verbose = true ){ _verbose = verbose; }

	void setInfinityStream( bool isInfinity ) { _infinityStream = isInfinity; }

	void setOffset( bool offset = true ){ _offset = offset; }

	/**
	 * @brief Get the duration of the stream.
	 * @note if it's a generated stream, return limit of double.
	 */
	double getDuration() const;

private:
	bool processRewrap();
	bool processRewrap( const int subStreamIndex );
	bool processTranscode();
	bool processTranscode( const int subStreamIndex );

private:
	IInputStream*     _inputStream;
	IOutputStream*    _outputStream;

	mediacore::Frame* _sourceBuffer;
	mediacore::Frame* _frameBuffer;

	IInputEssence*    _inputEssence;
	IInputEssence*    _generatorEssence;
	IInputEssence*    _currentEssence;
	IOutputEssence*   _outputEssence;

	mediatransform::ITransform* _transform;

	int  _subStreamIndex;

	/**
	 * @brief How many frame processed for this StreamTranscoder.
	 */
	size_t _frameProcessed;
	/**
	 * @brief Offset, in frame, at the beginning of the StreamTranscoder.
	 */
	size_t _offset;

	bool _takeFromGenerator;

	bool _verbose;

	bool _offsetPassed;

	/**
	 * @brief Automatic switch to a generator
	 * @note not applicable when rewrap
	 */
	bool _infinityStream;
};

}
}

#endif
