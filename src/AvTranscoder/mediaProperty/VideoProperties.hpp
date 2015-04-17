#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_VIDEO_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_VIDEO_PROPERTIES_HPP

#include "PixelProperties.hpp"

#include <AvTranscoder/mediaProperty/StreamProperties.hpp>
#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/progress/IProgress.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
}

#include <string>
#include <vector>
#include <utility>

namespace avtranscoder
{

class AvExport VideoProperties : public StreamProperties
{
public:
	VideoProperties( const FormatContext& formatContext, const size_t index, IProgress& progress, const EAnalyseLevel level = eAnalyseLevelFirstGop );

	std::string getCodecName() const;
	std::string getCodecLongName() const;
	std::string getProfileName() const;
	std::string getColorTransfert() const;
	std::string getColorspace() const;
	std::string getColorRange() const;
	std::string getColorPrimaries() const;
	std::string getChromaSampleLocation() const;
	std::string getFieldOrder() const;

	PixelProperties& getPixelProperties() { return _pixelProperties; }

	int64_t getStartTimecode() const;
	std::string getStartTimecodeString() const;

	Rational getTimeBase() const;
	Rational getSar() const; // sample/pixel aspect ratio
	Rational getDar() const; // display aspect ratio

	size_t getStreamId() const;
	size_t getCodecId() const;
	size_t getBitRate() const;  ///< in bits/s
	size_t getMaxBitRate() const;
	size_t getMinBitRate() const;
	size_t getNbFrames() const;
	size_t getTicksPerFrame() const;
	size_t getWidth() const;
	size_t getHeight() const;
	size_t getGopSize() const;
	size_t getDtgActiveFormat() const;
	size_t getReferencesFrames() const;
	int getProfile() const;
	int getLevel() const;

	double getFps() const;
	double getDuration() const;  ///< in seconds

	bool hasBFrames() const;
	//bool isClosedGop() const;

	//@{
	// Warning: Can acces these data when analyse first gop
	// @see EAnalyseLevel
	// @see analyseGopStructure
	bool isInterlaced() const { return _isInterlaced; }
	bool isTopFieldFirst() const { return _isTopFieldFirst; }
	std::vector< std::pair< char, bool > > getGopStructure() const { return _gopStructure; }
	//@}

#ifndef SWIG
	AVCodecContext& getAVCodecContext() { return *_codecContext; }
	const PixelProperties& getPixelProperties() const { return _pixelProperties; }
#endif

	PropertyVector getPropertiesAsVector() const;

private:
	/**
	 *  @brief frame type / is key frame
	 *  @param progress: callback to get analysis progression
	 */
	void analyseGopStructure( IProgress& progress );

#ifndef SWIG
	template<typename T>
	void addProperty( PropertyVector& dataVector, const std::string& key, T (VideoProperties::*getter)(void) const ) const
	{
		try
		{
		    detail::add( dataVector, key, (this->*getter)() );
		}
		catch( const std::exception& e )
		{
		    detail::add( dataVector, key, e.what() );
		}
	}
#endif

private:
	AVCodecContext* _codecContext;  ///< Has link (no ownership)
	AVCodec* _codec;  ///< Has link (no ownership)

	PixelProperties _pixelProperties;
	//@{
	// Can acces these data when analyse first gop
	bool _isInterlaced;
	bool _isTopFieldFirst;
	std::vector< std::pair< char, bool > > _gopStructure;
	//@}
};

}

#endif
