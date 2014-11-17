#include "VideoProperties.hpp"

#include <AvTranscoder/progress/NoDisplayProgress.hpp>

extern "C" {
#include <libavutil/avutil.h>
}

#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _MSC_VER
#include <float.h>
#define isnan _isnan
#define isinf(x) (!_finite(x))
#endif

namespace avtranscoder
{

VideoProperties::VideoProperties( const AVFormatContext* formatContext, const size_t index, IProgress& progress, const EAnalyseLevel level )
	: _formatContext( formatContext )
	, _codecContext( NULL )
	, _codec( NULL )
	, _pixFmt( NULL )
	, _streamId( index )
	, _isInterlaced( false )
	, _isTopFieldFirst( false )
	, _gopStructure()
{
	if( _formatContext )
		_codecContext = formatContext->streams[index]->codec;

	if( _formatContext && _codecContext )
		_codec = avcodec_find_decoder( _codecContext->codec_id );

	// Skip decoding for selected frames
	_codecContext->skip_frame = AVDISCARD_NONE;

	// Get pixel format
#if LIBAVUTIL_VERSION_MAJOR > 51
	_pixFmt = av_pix_fmt_desc_get( _codecContext->pix_fmt );
#else
	_pixFmt = NULL;
	if( _codecContext->pix_fmt >= 0 && _codecContext->pix_fmt < PIX_FMT_NB )
		_pixFmt = &av_pix_fmt_descriptors[ _codecContext->pix_fmt ];
#endif

	if( level == eAnalyseLevelFirstGop )
		analyseGopStructure( progress );
}

std::string VideoProperties::getCodecName() const
{
	if( _codecContext && _codec )
	{
		if( _codec->capabilities & CODEC_CAP_TRUNCATED )
			_codecContext->flags|= CODEC_FLAG_TRUNCATED;

		if( _codec->name )
			return std::string( _codec->name );
	}
	return "unknown codec";
}

std::string VideoProperties::getCodecLongName() const
{
	if( _codecContext && _codec )
	{
		if( _codec->capabilities & CODEC_CAP_TRUNCATED )
			_codecContext->flags|= CODEC_FLAG_TRUNCATED;

		if( _codec->long_name )
			return std::string( _codec->long_name );
	}
	return "unknown codec";
}

std::string VideoProperties::getProfileName() const
{
	if( _codecContext && _codec )
	{
		if( _codec->capabilities & CODEC_CAP_TRUNCATED )
			_codecContext->flags|= CODEC_FLAG_TRUNCATED;

		if( _codecContext->profile != -99 )
		{
			const char* profile;
			if( ( profile = av_get_profile_name( _codec, _codecContext->profile ) ) != NULL )
				return std::string( profile );
		}
	}
	return "unknown profile";
}

std::string VideoProperties::getColorTransfert() const
{
	switch( _codecContext->color_trc )
	{
		case AVCOL_TRC_BT709:
			return "Rec 709 / ITU-R BT1361";
		case AVCOL_TRC_UNSPECIFIED:
			return "unspecified";
		case AVCOL_TRC_GAMMA22:
			return "Gamma 2.2";
		case AVCOL_TRC_GAMMA28:
			return "Gamma 2.8";
#if LIBAVCODEC_VERSION_MAJOR > 53
		case AVCOL_TRC_SMPTE240M:
			return "Smpte 240M";
#endif
#if LIBAVCODEC_VERSION_MAJOR > 54
#ifdef AVCOL_TRC_SMPTE170M
		case AVCOL_TRC_SMPTE170M:
			return "Rec 601 / ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC";
#endif
#ifdef AVCOL_TRC_LINEAR
		case AVCOL_TRC_LINEAR:
			return "Linear transfer characteristics";
#endif
#ifdef AVCOL_TRC_LOG
		case AVCOL_TRC_LOG:
			return "Logarithmic transfer characteristic (100:1 range)";
#endif
#ifdef AVCOL_TRC_LOG_SQRT
		case AVCOL_TRC_LOG_SQRT:
			return "Logarithmic transfer characteristic (100 * Sqrt( 10 ) : 1 range)";
#endif
#ifdef AVCOL_TRC_IEC61966_2_4
		case AVCOL_TRC_IEC61966_2_4:
			return "IEC 61966-2-4";
#endif
#ifdef AVCOL_TRC_BT1361_ECG
		case AVCOL_TRC_BT1361_ECG:
			return "ITU-R BT1361 Extended Colour Gamut";
#endif
#ifdef AVCOL_TRC_IEC61966_2_1
		case AVCOL_TRC_IEC61966_2_1:
			return "IEC 61966-2-1 (sRGB or sYCC)";
#endif
#ifdef AVCOL_TRC_BT2020_10
		case AVCOL_TRC_BT2020_10:
			return "ITU-R BT2020 for 10 bit system";
#endif
#ifdef AVCOL_TRC_BT2020_12
		case AVCOL_TRC_BT2020_12:
			return "ITU-R BT2020 for 12 bit system";	
#endif
#endif
		case AVCOL_TRC_NB:
			return "Not ABI";
		default:
			return "";
	}
}

std::string VideoProperties::getColorspace() const
{
	switch( _codecContext->colorspace )
	{
		case AVCOL_SPC_RGB:
			return "RGB";
		case AVCOL_SPC_BT709:
			return "Rec 709";
		case AVCOL_SPC_UNSPECIFIED:
			return "unspecified";
		case AVCOL_SPC_FCC:
			return "Four CC";
		case AVCOL_SPC_BT470BG:
			return "BT470 (PAL - 625)";
		case AVCOL_SPC_SMPTE170M:
			return "Smpte 170M (NTSC)";
		case AVCOL_SPC_SMPTE240M:
			return "Smpte 240M";
#if LIBAVCODEC_VERSION_MAJOR > 53
		case AVCOL_SPC_YCOCG:
			return "Y Co Cg";
//#else
//		case AVCOL_SPC_YCGCO:
//		return "Y Cg Co";
#endif
#if LIBAVCODEC_VERSION_MAJOR > 54
#ifdef AVCOL_TRC_BT2020_12
		case AVCOL_SPC_BT2020_NCL:
			return "ITU-R BT2020 non-constant luminance system";
#endif
#ifdef AVCOL_TRC_BT2020_CL
		case AVCOL_SPC_BT2020_CL:
			return "ITU-R BT2020 constant luminance system";
#endif
#endif
		case AVCOL_SPC_NB:
			return "Not ABI";
		default:
			return "";
	}
}

std::string VideoProperties::getColorRange() const
{
	switch( _codecContext->color_range )
	{
		case AVCOL_RANGE_UNSPECIFIED:
			return "unspecified";
		case AVCOL_RANGE_MPEG:
			return "Head";
		case AVCOL_RANGE_JPEG:
			return "Full";
		case AVCOL_RANGE_NB:
			return "Not ABI";
		default:
			return "";
	}
}

std::string VideoProperties::getColorPrimaries() const
{
	switch( _codecContext->color_primaries )
	{
		case AVCOL_PRI_BT709:
			return "Rec 709";
		case AVCOL_PRI_UNSPECIFIED:
			return "unspecified";
		case AVCOL_PRI_BT470M:
			return "BT 470M";
		case AVCOL_PRI_BT470BG:
			return "Rec 601 (PAL & SECAM)";
		case AVCOL_PRI_SMPTE170M:
			return "Rec 601 (NTSC)";
		case AVCOL_PRI_SMPTE240M:
			return "Smpte 240 (NTSC)";
		case AVCOL_PRI_FILM:
			return "Film";
#if LIBAVCODEC_VERSION_MAJOR > 54
#ifdef AVCOL_TRC_BT2020_CL
		case AVCOL_PRI_BT2020:
			return "ITU-R BT2020";
#endif
#endif
		case AVCOL_PRI_NB:
			return "Not ABI";
		default:
			return "";
	}
}

std::string VideoProperties::getChromaSampleLocation() const
{
	switch( _codecContext->chroma_sample_location )
	{
		case AVCHROMA_LOC_UNSPECIFIED:
			return "unspecified";
		case AVCHROMA_LOC_LEFT:
			return "left (mpeg2/4, h264 default)";
		case AVCHROMA_LOC_CENTER:
			return "center (mpeg1, jpeg, h263)";
		case AVCHROMA_LOC_TOPLEFT:
			return "top left";
		case AVCHROMA_LOC_TOP:
			return "top";
		case AVCHROMA_LOC_BOTTOMLEFT:
			return "bottom left";
		case AVCHROMA_LOC_BOTTOM:
			return "bottom";
		case AVCHROMA_LOC_NB:
			return "Not ABI";
		default:
			return "";
	}
}

std::string VideoProperties::getFieldOrder() const
{
	switch( _codecContext->field_order )
	{
		case AV_FIELD_UNKNOWN:
			return "unknown";
		case AV_FIELD_PROGRESSIVE:
			return "progressive";
		case AV_FIELD_TT:
			return "top top";
		case AV_FIELD_BB:
			return "bottom bottom";
		case AV_FIELD_TB:
			return "top bottom";
		case AV_FIELD_BT:
			return "bottom top";
		default:
			return "";
	}
}

std::string VideoProperties::getEndianess() const
{
	return ( _pixFmt->flags & PIX_FMT_BE ) ? "big" : "little";
}

std::string VideoProperties::getStartTimecodeString() const
{
	int64_t stratTimeCode = getStartTimecode();
	std::ostringstream os;
	os << std::setfill( '0' );
	os << std::setw(2) << ( stratTimeCode >> 19 & 0x1f ) << ":";   // 5-bit hours
	os << std::setw(2) << ( stratTimeCode >> 13 & 0x3f ) << ":";   // 6-bit minutes
	os << std::setw(2) << ( stratTimeCode >>  6 & 0x3f ) ;         // 6-bit seconds
	os << ( stratTimeCode & 1 << 24 ? ';' : ':' ); // 1-bit drop flag
	os << std::setw(2) << ( stratTimeCode       & 0x3f );   // 6-bit frames
	return os.str();
}


Rational VideoProperties::getTimeBase() const
{
	Rational timeBase;
	timeBase.num = _codecContext->time_base.num;
	timeBase.den = _codecContext->time_base.den;
	return timeBase;
}

Rational VideoProperties::getSar() const
{
	Rational sar;
	sar.num = _codecContext->sample_aspect_ratio.num;
	sar.den = _codecContext->sample_aspect_ratio.den;
	return sar;
}

Rational VideoProperties::getDar() const
{
	int darNum, darDen;
	av_reduce( &darNum, &darDen,
			   _codecContext->width * getSar().num,
			   _codecContext->height * getSar().den,
			   1024 * 1024);

	Rational dar;
	dar.num = darNum;
	dar.den = darDen;
	return dar;
}

double VideoProperties::getFps() const
{
	Rational timeBase = getTimeBase();
	double fps = 1.0 * timeBase.den / ( timeBase.num * getTicksPerFrame() );
	if( isinf( fps ) )
		fps = 0.0;
	return fps;
}

bool VideoProperties::isPseudoPaletted() const
{
#if LIBAVCODEC_VERSION_MAJOR > 53
	return (bool) _pixFmt->flags & PIX_FMT_PSEUDOPAL;
#else
	return false;
#endif
}

bool VideoProperties::hasAlpha() const
{
#if LIBAVCODEC_VERSION_MAJOR > 53
	return (bool) _pixFmt->flags & PIX_FMT_ALPHA;
#else
	return false;
#endif
}

void VideoProperties::analyseGopStructure( IProgress& progress )
{
	if( _formatContext && _codecContext && _codec )
	{
		if( _codec->capabilities & CODEC_CAP_TRUNCATED )
			_codecContext->flags|= CODEC_FLAG_TRUNCATED;

		if( _codecContext->width && _codecContext->height )
		{
			AVPacket pkt;

#if LIBAVCODEC_VERSION_MAJOR > 54
			AVFrame* frame = av_frame_alloc();
#else
			AVFrame* frame = avcodec_alloc_frame();
#endif

			av_init_packet( &pkt );
			avcodec_open2( _codecContext, _codec, NULL );

			int count = 0;
			int gotFrame = 0;
			bool stopAnalyse = false;

			while( ! av_read_frame( const_cast<AVFormatContext*>( _formatContext ), &pkt ) )
			{
				if( pkt.stream_index == (int)_streamId )
				{
					avcodec_decode_video2( _codecContext, frame, &gotFrame, &pkt );
					if( gotFrame )
					{
						_gopStructure.push_back( std::make_pair( av_get_picture_type_char( frame->pict_type ), frame->key_frame ) );
						_isInterlaced  = frame->interlaced_frame;
						_isTopFieldFirst = frame->top_field_first;
						++count;
						if( progress.progress( count, _codecContext->gop_size ) == eJobStatusCancel )
							stopAnalyse = true;
					}
				}

				av_free_packet( &pkt );

				if( _codecContext->gop_size == count )
				{
					stopAnalyse = true;
				}

				if( stopAnalyse )
					break;
			}
#if LIBAVCODEC_VERSION_MAJOR > 54
			av_frame_free( &frame );
#else
#if LIBAVCODEC_VERSION_MAJOR > 53
			avcodec_free_frame( &frame );
#else
			av_free( frame );
#endif
#endif
		}
	}
}

std::vector<Channel> VideoProperties::getChannels() const
{
	std::vector<Channel> channels;
	for( size_t channel = 0; channel < (size_t)_pixFmt->nb_components; ++channel )
	{
		Channel c;
		c.id = channel;
		c.chromaHeight = (size_t)_pixFmt->comp[channel].plane;
		c.bitStep = (size_t)_pixFmt->comp[channel].step_minus1;
		channels.push_back( c );
	}
	return channels;
}

MetadatasMap VideoProperties::getDataMap() const
{
	MetadatasMap dataMap;

	detail::add( dataMap, "stream id", getStreamId() );
	detail::add( dataMap, "codec id", getCodecId() );
	detail::add( dataMap, "codec name", getCodecName() );
	detail::add( dataMap, "codec long name", getCodecLongName() );
	detail::add( dataMap, "profile", getProfile() );
	detail::add( dataMap, "profile name", getProfileName() );
	detail::add( dataMap, "level", getLevel() );
	detail::add( dataMap, "start timecode", getStartTimecode() );
	detail::add( dataMap, "width", getWidth() );
	detail::add( dataMap, "height", getHeight() );
	detail::add( dataMap, "pixel aspect ratio", getSar().num / getSar().den );
	detail::add( dataMap, "display aspect ratio", getDar().num / getDar().den );
	detail::add( dataMap, "dtgActiveFormat", getDtgActiveFormat() );
	detail::add( dataMap, "components count", getComponentsCount() );
	detail::add( dataMap, "pixel type", getPixelName() );
	detail::add( dataMap, "bit wise acked", isBitWisePacked() );
	detail::add( dataMap, "rgb pixel", isRgbPixelData() );
	detail::add( dataMap, "as alpha", hasAlpha() );
	detail::add( dataMap, "chroma width", getChromaWidth() );
	detail::add( dataMap, "chroma height", getChromaHeight() );
	detail::add( dataMap, "endianess", getEndianess() );
	detail::add( dataMap, "color transfert", getColorTransfert() );
	detail::add( dataMap, "colorspace", getColorspace() );
	detail::add( dataMap, "color range", getColorRange() );
	detail::add( dataMap, "color primaries", getColorPrimaries() );
	detail::add( dataMap, "indexed colors", isIndexedColors() );
	detail::add( dataMap, "pseudo paletted", isPseudoPaletted() );
	detail::add( dataMap, "chroma sample location", getChromaSampleLocation() );
	detail::add( dataMap, "interlaced ", isInterlaced() );
	detail::add( dataMap, "top field first", isTopFieldFirst() );
	detail::add( dataMap, "field order", getFieldOrder() );
	detail::add( dataMap, "timeBase", getTimeBase().num / getTimeBase().den );
	detail::add( dataMap, "fps", getFps() );
	detail::add( dataMap, "ticksPerFrame", getTicksPerFrame() );
	detail::add( dataMap, "bit rate", getBitRate() );
	detail::add( dataMap, "max bit rate", getMaxBitRate() );
	detail::add( dataMap, "min bit rate", getMinBitRate() );
	detail::add( dataMap, "gop size", getGopSize() );

	std::string gop;
	NoDisplayProgress progress;
	std::vector< std::pair< char, bool > > gopStructure = getGopStructure();
	for( size_t frameIndex = 0; frameIndex < gopStructure.size(); ++frameIndex )
	{
		gop += gopStructure.at( frameIndex ).first;
		gop += ( gopStructure.at( frameIndex ).second ? "*" : " " );
	}
	detail::add( dataMap, "gop", gop );

	detail::add( dataMap, "has B frames", hasBFrames() );
	detail::add( dataMap, "references frames", getReferencesFrames() );

	for( size_t metadataIndex = 0; metadataIndex < _metadatas.size(); ++metadataIndex )
	{
		detail::add( dataMap, _metadatas.at( metadataIndex ).first, _metadatas.at( metadataIndex ).second );
	}

	return dataMap;
}

}
