#ifndef _AV_TRANSCODER_OPTION_LOADER_HPP
#define	_AV_TRANSCODER_OPTION_LOADER_HPP


#include <string>
#include <vector>
#include <map>
#include <utility> //pair

#include "common.hpp"
#include "Option.hpp"

class AVFormatContext;
class AVCodecContext;
class AVOutputFormat;
class AVCodec;

namespace avtranscoder
{

/**
 * @brief Manage Options.
 * Get a list of options by calling a load****Options(), depending on which FFMpeg / LibAV object you want to analyse.
 */
class OptionLoader 
{
public:
	typedef std::vector<Option> OptionArray;
	typedef std::map< std::string, std::vector<Option> > OptionMap;
	
public:
	OptionLoader();
	~OptionLoader();
	
	const AVFormatContext* getFormatContext() const { return _avFormatContext; }
	const AVCodecContext* getCodecContext() const { return _avCodecContext; }
	const AVOutputFormat* getOutputFormat() const { return _outputFormat; }
	const AVCodec* getCodec() const { return _codec; }
		
	/**
	 * @param req_flags: AVOption flags (default = 0: no flag restriction)
	 */
	OptionArray loadFormatContextOptions( int req_flags = 0 );
	
	/**
	 * @param req_flags: AVOption flags (default = 0: no flag restriction)
	 */
	OptionArray loadCodecContextOptions( int req_flags = 0 );
	
	OptionMap loadOutputFormatOptions();
	OptionMap loadVideoCodecOptions();
	OptionMap loadAudioCodecOptions();
	
	std::vector<std::string>& getFormatsLongNames() { return _formatsLongNames; }
	std::vector<std::string>& getFormatsShortNames() { return _formatsShortNames; }
	
	std::vector<std::string>& getVideoCodecsLongNames() { return _videoCodecsLongNames; }
	std::vector<std::string>& getVideoCodecsShortNames() { return _videoCodecsShortNames; }
	
	std::vector<std::string>& getAudioCodecsLongNames() { return _audioCodecsLongNames; }
	std::vector<std::string>& getAudioCodecsShortNames() { return _audioCodecsShortNames; }

public:
	/**
	 *  @brief Get array of pixel format supported by video codec.
	 *  @param videoCodecName: the video codec name (empty if not indicated, and so get all pixel formats supported by all video codecs).
	 */
	static std::vector<std::string> getPixelFormats( const std::string& videoCodecName = "" );
	
	/**
	 *  @brief Get array of sample format supported by an audio codec.
	 *  @param audioCodecName: the audio codec name (empty if not indicated, and so get all sample formats supported by all audio codecs).
	 */
	static std::vector<std::string> getSampleFormats( const std::string& audioCodecName = "" );
	
	/**
	 * @brief Get the corresponding AVPixelFormat from the pixel format name
	 * @param pixelFormat the name of the pixel format
     */
	static AVPixelFormat getAVPixelFormat( const std::string& pixelFormat );
	
	/**
	 * @brief Get the corresponding AVSampleFormat from the sample format name
	 * @param sampleFormat the name of the sample format
     */
	static AVSampleFormat getAVSampleFormat( const std::string& sampleFormat );
	
private:
	/**
	 * @brief: load array of Option depending on the flags.
	 * @param req_flags: AVOption flags we want to load.
	 */
	OptionArray loadOptions( void* av_class, int req_flags = 0 );
	
	AVFormatContext* _avFormatContext;
	AVCodecContext* _avCodecContext;
	
	AVOutputFormat* _outputFormat;
	AVCodec* _codec;
		
	std::vector<std::string> _formatsLongNames;
	std::vector<std::string> _formatsShortNames;
	
	std::vector<std::string> _videoCodecsLongNames;
	std::vector<std::string> _videoCodecsShortNames;

	std::vector<std::string> _audioCodecsLongNames;
	std::vector<std::string> _audioCodecsShortNames;
};

}

#endif
