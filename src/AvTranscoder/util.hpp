#ifndef _AV_TRANSCODER_UTIL_HPP
#define	_AV_TRANSCODER_UTIL_HPP

#include "common.hpp"
#include "Option.hpp"

extern "C" {
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>
}

#include <string>
#include <vector>
#include <map>

namespace avtranscoder
{

typedef std::map<std::string, OptionArray> OptionArrayMap;
typedef std::vector< std::pair<std::string, std::string> > NamesArray;  //< short/long names of format/video codec/audio codec

/**
 * @brief Get format name from a given filename 
 */
std::string AvExport getFormat( const std::string& filename );

/**
 * @brief Check if a format name corresponds to the format of a given filename
 */
bool AvExport matchFormat( const std::string& format, const std::string& filename );

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
 * @brief Get array of short/long names of all format supported by FFmpeg / libav.
 */
NamesArray getFormatsNames();

/**
 * @brief Get array of short/long names of all video codec supported by FFmpeg / libav.
 */
NamesArray getVideoCodecsNames();

/**
 * @brief Get array of short/long names of all audio codec supported by FFmpeg / libav.
 */
NamesArray getAudioCodecsNames();

/**
 * @brief Get the list of options for each output format
 */
OptionArrayMap getOutputFormatOptions();

/**
 * @brief Get the list of options for each video codec
 */
OptionArrayMap getVideoCodecOptions();

/**
 * @brief Get the list of options for each audio codec
 */
OptionArrayMap getAudioCodecOptions();

}

#endif
