#ifndef _AV_TRANSCODER_UTIL_HPP
#define _AV_TRANSCODER_UTIL_HPP

#include "common.hpp"
#include "Option.hpp"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>
}

#include <string>
#include <vector>
#include <map>

namespace avtranscoder
{

typedef std::map<std::string, OptionArray> OptionArrayMap;
typedef std::map<std::string, std::string> NamesMap; //< short/long names of format/video codec/audio codec

/**
*  @brief Get pixel format supported by a video codec.
*  @param videoCodecName: the video codec name (empty if not indicated, and so get all pixel formats supported by all video
* codecs).
*/
std::vector<std::string> AvExport getSupportedPixelFormats(const std::string& videoCodecName = "");

/**
*  @brief Get sample format supported by an audio codec.
*  @param audioCodecName: the audio codec name (empty if not indicated, and so get all sample formats supported by all audio
* codecs).
*/
std::vector<std::string> AvExport getSupportedSampleFormats(const std::string& audioCodecName = "");

/**
* @brief Get the corresponding AVPixelFormat from the pixel format name
* @param pixelFormat: the name of the pixel format
*/
AVPixelFormat AvExport getAVPixelFormat(const std::string& pixelFormat);

/**
* @brief Get the corresponding AVSampleFormat from the sample format name
* @param sampleFormat: the name of the sample format
*/
AVSampleFormat AvExport getAVSampleFormat(const std::string& sampleFormat);

/**
 * @return The name of the given pixel format.
 * @note Returns an empty string if the format is not found.
 */
std::string AvExport getPixelFormatName(const AVPixelFormat pixelFormat);

/**
 * @return The name of the given sample format.
 * @note Returns an empty string if the format is not found.
 */
std::string AvExport getSampleFormatName(const AVSampleFormat sampleFormat);

#ifndef SWIG
/**
 * @return The list of all formats available by FFmpeg / libav.
 */
std::vector<AVOutputFormat*> getAvailableFormats();
#endif
/**
 * @brief Get a map of short/long names of all formats available by FFmpeg / libav.
 * @note Need to call preloadCodecsAndFormats before using this function.
 */
NamesMap AvExport getAvailableFormatsNames();

/**
 * @brief Get a map of short/long names of all formats dedicate for video available by FFmpeg / libav.
 * @note Need to call preloadCodecsAndFormats before using this function.
 */
NamesMap AvExport getAvailableVideoFormatsNames();

/**
 * @brief Get a map of short/long names of all formats dedicate for video available by FFmpeg / libav.
 * @note Need to call preloadCodecsAndFormats before using this function.
 */
NamesMap AvExport getAvailableAudioFormatsNames();

#ifndef SWIG
/**
 * @return The list of all codecs available by FFmpeg / libav.
 */
std::vector<AVCodec*> getAvailableCodecs();
#endif

/**
 * @brief Get a map of short/long names of all video codecs available by FFmpeg / libav.
 * @note Need to call preloadCodecsAndFormats before using this function.
 */
NamesMap AvExport getAvailableVideoCodecsNames();

/**
 * @brief Get a map of short/long names of all audio codecs available by FFmpeg / libav.
 * @note Need to call preloadCodecsAndFormats before using this function.
 */
NamesMap AvExport getAvailableAudioCodecsNames();

#ifndef SWIG
/**
 * @brief Get the list of options for each output format
 * @note Need to call preloadCodecsAndFormats before using this function.
 */
OptionArrayMap AvExport getAvailableOptionsPerOutputFormat();

/**
 * @brief Get the list of options for each video codec
 * @note Need to call preloadCodecsAndFormats before using this function.
 */
OptionArrayMap AvExport getAvailableOptionsPerVideoCodec();

/**
 * @brief Get the list of options for each audio codec
 * @note Need to call preloadCodecsAndFormats before using this function.
 */
OptionArrayMap AvExport getAvailableOptionsPerAudioCodec();
#endif
}

#endif
