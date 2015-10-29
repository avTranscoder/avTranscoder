#ifndef _AV_TRANSCODER_FILE_IOUTPUT_FILE_HPP_
#define _AV_TRANSCODER_FILE_IOUTPUT_FILE_HPP_

#include <AvTranscoder/common.hpp>

#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/codec/AudioCodec.hpp>
#include <AvTranscoder/codec/DataCodec.hpp>

#include <AvTranscoder/stream/OutputStream.hpp>

#include <string>
#include <stdexcept>

namespace avtranscoder
{

/**
 * @brief IOutputfile is the interface to wrap and write medias.
 * It can be overloaded to integrate custom wrapper.
 **/
class AvExport IOutputFile
{
public:
	virtual ~IOutputFile() {};

	/**
	 * @brief Add a video output stream
	 * @param videoCodec description of output stream
	**/
	virtual IOutputStream& addVideoStream( const VideoCodec& videoCodec )
	{
	    throw std::logic_error("function is not implemented");
	}

	/**
	 * @brief Add an audio output stream
	 * @param audioCodec description of output stream
	**/
	virtual IOutputStream& addAudioStream( const AudioCodec& audioCodec )
	{
	    throw std::logic_error("function is not implemented");
	}

	/**
	 * @brief Add a data output stream
	 * @param dataCodec description of output stream
	**/
	virtual IOutputStream& addDataStream( const DataCodec& dataCodec )
	{
	    throw std::logic_error("function is not implemented");
	}

	/**
	 * @brief Write the header of file (if necessary)
	**/
	virtual bool beginWrap() = 0;

	/**
	 * @brief Wrap a packet of data in the output ressource
	 * @param data coded packet information for the current stream
	 * @param streamIndex refers to the stream in output ressource
	 * @return the wrapping status after wrapping
	 * @see EWrappingStatus
	**/
	virtual IOutputStream::EWrappingStatus wrap( const CodedData& data, const size_t streamIndex ) = 0;

	/**
	 * @brief Write the footer of file (if necessary)
	**/
	virtual bool endWrap() = 0;
	
	/**
	 * @brief Get the output stream
	 * @param streamIndex select the output stream
	 * @return the output stream reference
	**/
	virtual IOutputStream& getStream( const size_t streamIndex ) = 0;
};

}

#endif
