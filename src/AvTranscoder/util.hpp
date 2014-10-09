#ifndef _AV_TRANSCODER_UTIL_HPP
#define	_AV_TRANSCODER_UTIL_HPP

#include "common.hpp"

extern "C" {
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>
}

#include <string>
#include <vector>
#include <map>

namespace avtranscoder
{

class Option;

/**
*  @brief Get pixel format supported by a video codec.
*  @param videoCodecName: the video codec name (empty if not indicated, and so get all pixel formats supported by all video codecs).
*/
std::vector<std::string> getPixelFormats( const std::string& videoCodecName = "" );

/**
*  @brief Get sample format supported by an audio codec.
*  @param audioCodecName: the audio codec name (empty if not indicated, and so get all sample formats supported by all audio codecs).
*/
std::vector<std::string> getSampleFormats( const std::string& audioCodecName = "" );

/**
* @brief Get the corresponding AVPixelFormat from the pixel format name
* @param pixelFormat: the name of the pixel format
*/
AVPixelFormat getAVPixelFormat( const std::string& pixelFormat );

/**
* @brief Get the corresponding AVSampleFormat from the sample format name
* @param sampleFormat: the name of the sample format
*/
AVSampleFormat getAVSampleFormat( const std::string& sampleFormat );

/**
 * @brief Get long name of all format supported by FFmpeg / libav.
 * @see getFormatsShortNames: to get short names
 */
std::vector<std::string> getFormatsLongNames();
std::vector<std::string> getFormatsShortNames();

/**
 * @brief Get long name of all video codec supported by FFmpeg / libav.
 * @see getVideoCodecsShortNames: to get short names
 */
std::vector<std::string> getVideoCodecsLongNames();
std::vector<std::string> getVideoCodecsShortNames();

/**
 * @brief Get long name of all audio codec supported by FFmpeg / libav.
 * @see getAudioCodecsShortNames: to get short names
 */
std::vector<std::string> getAudioCodecsLongNames();
std::vector<std::string> getAudioCodecsShortNames();

/**
 * @brief Get the list of options for each output format
 */
std::map< std::string, std::vector<Option> > getOutputFormatOptions();

/**
 * @brief Get the list of options for each video codec
 */
std::map< std::string, std::vector<Option> > getVideoCodecOptions();

/**
 * @brief Get the list of options for each audio codec
 */
std::map< std::string, std::vector<Option> > getAudioCodecOptions();

}

#endif
