#ifndef _AV_TRANSCODER_MEDIA_PROPERTY_VIDEO_PROPERTIES_HPP
#define _AV_TRANSCODER_MEDIA_PROPERTY_VIDEO_PROPERTIES_HPP

#include <AvTranscoder/common.hpp>
#include <AvTranscoder/mediaProperty/util.hpp>
#include <AvTranscoder/file/util.hpp>
#include <AvTranscoder/progress/IProgress.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixdesc.h>
}

#include <string>
#include <vector>
#include <utility>

namespace avtranscoder
{

struct AvExport Channel
{
	size_t id;
	size_t chromaHeight;
	size_t bitStep;
};

class AvExport VideoProperties
{
public:
	VideoProperties( const AVFormatContext* formatContext, const size_t index, IProgress& progress, const EAnalyseLevel level = eAnalyseLevelFirstGop );

	std::string getCodecName() const;
	std::string getCodecLongName() const;
	std::string getProfileName() const;
	std::string getColorTransfert() const;
	std::string getColorspace() const;
	std::string getColorRange() const;
	std::string getColorPrimaries() const;
	std::string getChromaSampleLocation() const;
	std::string getFieldOrder() const;

	std::string getPixelName() const { return _pixFmt->name; }
	std::string getEndianess() const;

	int64_t getStartTimecode() const { return _codecContext->timecode_frame_start; }
	std::string getStartTimecodeString() const;

	Rational getTimeBase() const;
	Rational getSar() const; // sample/pixel aspect ratio
	Rational getDar() const; // display aspect ratio

	size_t getStreamId() const { return _streamId; }
	size_t getCodecId() const { return _codecContext->codec_id; }
	size_t getBitRate() const { return _codecContext->bit_rate; }
	size_t getMaxBitRate() const { return _codecContext->rc_max_rate; }
	size_t getMinBitRate() const { return _codecContext->rc_min_rate; }
	size_t getTicksPerFrame() const { return _codecContext->ticks_per_frame; }
	size_t getWidth() const { return _codecContext->width; }
	size_t getHeight() const { return _codecContext->height; }
	size_t getGopSize() const { return _codecContext->gop_size; }
	size_t getDtgActiveFormat() const { return _codecContext->dtg_active_format; }
	size_t getReferencesFrames() const { return _codecContext->refs; }
	int getProfile() const { return _codecContext->profile; }
	int getLevel() const { return _codecContext->level; }
	size_t getComponentsCount() const { return _pixFmt->nb_components; }
	size_t getChromaWidth() const { return _pixFmt->log2_chroma_w; }
	size_t getChromaHeight() const { return _pixFmt->log2_chroma_h; }

	double getFps() const;

	bool hasBFrames() const { return (bool) _codecContext->has_b_frames; }
	bool isIndexedColors() const { return (bool) _pixFmt->flags & PIX_FMT_PAL; }
	bool isBitWisePacked() const { return (bool) _pixFmt->flags & PIX_FMT_BITSTREAM; }
	bool isHardwareAccelerated() const { return (bool) _pixFmt->flags & PIX_FMT_HWACCEL; }
	bool isPlanar() const { return (bool) _pixFmt->flags & PIX_FMT_PLANAR; }
	bool isRgbPixelData() const { return (bool) _pixFmt->flags & PIX_FMT_RGB; }
	bool isPseudoPaletted() const;
	bool hasAlpha() const;

	//@{
	// Warning: Can acces these data when analyse first gop
	// Construct the VideoProperties with level = eAnalyseLevelFull
	// @see EAnalyseLevel
	// @see analyseGopStructure
	bool isInterlaced() const { return _isInterlaced; }
	bool isTopFieldFirst() const { return _isTopFieldFirst; }
	std::vector< std::pair< char, bool > > getGopStructure() const { return _gopStructure; }
	//@}

	std::vector<Channel> getChannels() const;

	MetadatasMap& getMetadatas() { return _metadatas; }

	const AVFormatContext& getAVFormatContext() { return *_formatContext; }
	AVCodecContext& getAVCodecContext() { return *_codecContext; }
	const AVPixFmtDescriptor& getAVPixFmtDescriptor() { return *_pixFmt; }

	MetadatasMap getDataMap() const;  ///< Warning: the method calls analyseGopStructure, which can modify state of the object

private:
	/**
	 *  @brief frame type / is key frame
	 *  @param progress: callback to get analysis progression
	 */
	void analyseGopStructure( IProgress& progress );

private:
	const AVFormatContext* _formatContext;  ///< Has link (no ownership)
	AVCodecContext* _codecContext;  ///< Has link (no ownership)
	AVCodec* _codec;  ///< Has link (no ownership)
	const AVPixFmtDescriptor* _pixFmt;  ///< Has link (no ownership)

	size_t _streamId;
	//@{
	// Can acces these data when analyse first gop
	bool _isInterlaced;
	bool _isTopFieldFirst;
	std::vector< std::pair< char, bool > > _gopStructure;
	//@}
	MetadatasMap _metadatas;
};

}

#endif
