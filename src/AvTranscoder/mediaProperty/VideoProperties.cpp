#include "VideoProperties.hpp"

#include <AvTranscoder/progress/NoDisplayProgress.hpp>

extern "C" {
#include <libavutil/avutil.h>
}

#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cmath>

namespace avtranscoder
{

VideoProperties::VideoProperties( const FormatContext& formatContext, const size_t index, IProgress& progress, const EAnalyseLevel level )
	: StreamProperties( formatContext, index )
	, _codecContext( NULL )
	, _codec( NULL )
	, _pixelProperties()
	, _isInterlaced( false )
	, _isTopFieldFirst( false )
	, _gopStructure()
	, _firstGopTimeCode( -1 )
{
	if( _formatContext )
	{
		if( _streamIndex > _formatContext->nb_streams )
		{
			std::stringstream ss;
			ss << "video stream at index " << _streamIndex << " does not exist";
			throw std::runtime_error( ss.str() );
		}
		_codecContext = _formatContext->streams[_streamIndex]->codec;
	}

	if( _formatContext && _codecContext )
		_codec = avcodec_find_decoder( _codecContext->codec_id );

	if( _codecContext )
	{
		_pixelProperties = PixelProperties( _codecContext->pix_fmt );
		_firstGopTimeCode = _codecContext->timecode_frame_start;
	}

	if( level == eAnalyseLevelFirstGop )
		analyseGopStructure( progress );
}

std::string VideoProperties::getCodecName() const
{
	if( ! _codecContext || ! _codec )
		throw std::runtime_error( "unknown codec" );

	if( _codec->capabilities & CODEC_CAP_TRUNCATED )
		_codecContext->flags|= CODEC_FLAG_TRUNCATED;

	if( ! _codec->name )
		throw std::runtime_error( "unknown codec name" );

	return std::string( _codec->name );
}

std::string VideoProperties::getCodecLongName() const
{
	if( ! _codecContext || ! _codec )
		throw std::runtime_error( "unknown codec" );

	if( _codec->capabilities & CODEC_CAP_TRUNCATED )
		_codecContext->flags|= CODEC_FLAG_TRUNCATED;

	if( ! _codec->long_name )
		throw std::runtime_error( "unknown codec long name" );

	return std::string( _codec->long_name );
}

std::string VideoProperties::getProfileName() const
{
	if( ! _codecContext || ! _codec )
		throw std::runtime_error( "unknown codec" );

	if( _codec->capabilities & CODEC_CAP_TRUNCATED )
		_codecContext->flags|= CODEC_FLAG_TRUNCATED;

	if( _codecContext->profile == -99 )
		throw std::runtime_error( "unknown codec profile" );

	const char* profile = NULL;
	if( ( profile = av_get_profile_name( _codec, _codecContext->profile ) ) == NULL )
		throw std::runtime_error( "unknown codec profile" );

	return std::string( profile );
}

std::string VideoProperties::getColorTransfert() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

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
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

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
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

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
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

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
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

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
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

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

int64_t VideoProperties::getStartTimecode() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _firstGopTimeCode;
}

std::string VideoProperties::getStartTimecodeString() const
{
	int64_t startTimeCode = getStartTimecode();
	std::ostringstream os;
	if( startTimeCode == -1 )
		os << "unset";
	else
	{
		os << std::setfill( '0' );
		os << std::setw(2) << ( startTimeCode >> 19 & 0x1f ) << ":";   // 5-bit hours
		os << std::setw(2) << ( startTimeCode >> 13 & 0x3f ) << ":";   // 6-bit minutes
		os << std::setw(2) << ( startTimeCode >>  6 & 0x3f ) ;         // 6-bit seconds
		os << ( startTimeCode & 1 << 24 ? ';' : ':' ); // 1-bit drop flag
		os << std::setw(2) << ( startTimeCode       & 0x3f );   // 6-bit frames
	}
	return os.str();
}

Rational VideoProperties::getSar() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

	Rational sar = {
		_codecContext->sample_aspect_ratio.num,
		_codecContext->sample_aspect_ratio.den,
	};
	return sar;
}

Rational VideoProperties::getDar() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );

	int darNum, darDen;
	av_reduce( &darNum, &darDen,
			   _codecContext->width * getSar().num,
			   _codecContext->height * getSar().den,
			   1024 * 1024);

	Rational dar = {
		darNum,
		darDen,
	};
	return dar;
}

size_t VideoProperties::getCodecId() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->codec_id;
}

size_t VideoProperties::getBitRate() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	// return bit rate of stream if present or VBR mode
	if( _codecContext->bit_rate || _codecContext->rc_max_rate )
		return _codecContext->bit_rate;
	
	// else compute bit rate from the first GOP
	if( ! _formatContext || ! _codec )
		throw std::runtime_error( "cannot compute bit rate: unknown format or codec context" );
	
	if( ! _codecContext->width || ! _codecContext->height )
		throw std::runtime_error( "cannot compute bit rate: invalid frame size" );

	// discard no frame type when decode
	_codecContext->skip_frame = AVDISCARD_NONE;

#if LIBAVCODEC_VERSION_MAJOR > 54
	AVFrame* frame = av_frame_alloc();
#else
	AVFrame* frame = avcodec_alloc_frame();
#endif
	AVPacket pkt;
	av_init_packet( &pkt );
	avcodec_open2( _codecContext, _codec, NULL );
	
	int gotFrame = 0;
	int count = 0;
	int gopFramesSize = 0;

	while( ! av_read_frame( const_cast<AVFormatContext*>( _formatContext ), &pkt ) )
	{
		if( pkt.stream_index == (int)_streamIndex )
		{
			avcodec_decode_video2( _codecContext, frame, &gotFrame, &pkt );
			if( gotFrame )
			{
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT( 54, 7, 100 )
				gopFramesSize += av_frame_get_pkt_size( frame );
#else
				gopFramesSize += pkt.size;
#endif
				++count;
			}
		}
		av_free_packet( &pkt );
		if( _codecContext->gop_size == count )
			break;
	}
#if LIBAVCODEC_VERSION_MAJOR > 54
	av_frame_free( &frame );
#elif LIBAVCODEC_VERSION_MAJOR > 53
	avcodec_free_frame( &frame );
#else
	av_free( frame );
#endif
	
	int bitsPerByte = 8;
	return (gopFramesSize / _codecContext->gop_size) * bitsPerByte * getFps();
}

size_t VideoProperties::getMaxBitRate() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->rc_max_rate;
}

size_t VideoProperties::getMinBitRate() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->rc_min_rate;
}

size_t VideoProperties::getNbFrames() const
{
	if( ! _formatContext )
		throw std::runtime_error( "unknown format context" );
	size_t nbFrames = _formatContext->streams[_streamIndex]->nb_frames;
	if( nbFrames == 0 )
		nbFrames = getFps() * getDuration();
	return nbFrames;
}

size_t VideoProperties::getTicksPerFrame() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->ticks_per_frame;
}

size_t VideoProperties::getWidth() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->width;
}

size_t VideoProperties::getHeight() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->height;
}

size_t VideoProperties::getGopSize() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->gop_size;
}

size_t VideoProperties::getDtgActiveFormat() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->dtg_active_format;
}

size_t VideoProperties::getReferencesFrames() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->refs;
}

int VideoProperties::getProfile() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->profile;
}

int VideoProperties::getLevel() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return _codecContext->level;
}

float VideoProperties::getFps() const
{
	return av_q2d( _formatContext->streams[_streamIndex]->avg_frame_rate );
}

bool VideoProperties::hasBFrames() const
{
	if( ! _codecContext )
		throw std::runtime_error( "unknown codec context" );
	return (bool) _codecContext->has_b_frames;
}

// CODEC_FLAG_CLOSED_GOP is superior of INT_MAX, and _codecContext->flags is an int
// => Need a patch from FFmpeg
//bool VideoProperties::isClosedGop() const
//{
//	if( ! _codecContext )
//		throw std::runtime_error( "unknown codec context" );
//	return ( _codecContext->flags & CODEC_FLAG_CLOSED_GOP ) == CODEC_FLAG_CLOSED_GOP;
//}

void VideoProperties::analyseGopStructure( IProgress& progress )
{
	if( _formatContext && _codecContext && _codec )
	{
		if( _codecContext->width && _codecContext->height )
		{
			// Discard no frame type when decode
			_codecContext->skip_frame = AVDISCARD_NONE;

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
				if( pkt.stream_index == (int)_streamIndex )
				{
					avcodec_decode_video2( _codecContext, frame, &gotFrame, &pkt );
					if( gotFrame )
					{
						_gopStructure.push_back( std::make_pair( av_get_picture_type_char( frame->pict_type ), frame->key_frame ) );
						_isInterlaced = frame->interlaced_frame;
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

PropertyVector VideoProperties::getPropertiesAsVector() const
{
	PropertyVector data;

	// Add properties of base class
	PropertyVector basedProperty = StreamProperties::getPropertiesAsVector();
	data.insert( data.begin(), basedProperty.begin(), basedProperty.end() );

	addProperty( data, "codecId", &VideoProperties::getCodecId );
	addProperty( data, "codecName", &VideoProperties::getCodecName );
	addProperty( data, "codecLongName", &VideoProperties::getCodecLongName );
	addProperty( data, "profile", &VideoProperties::getProfile );
	addProperty( data, "profileName", &VideoProperties::getProfileName );
	addProperty( data, "level", &VideoProperties::getLevel );
	addProperty( data, "startTimecode", &VideoProperties::getStartTimecodeString );
	addProperty( data, "width", &VideoProperties::getWidth );
	addProperty( data, "height", &VideoProperties::getHeight );
	addProperty( data, "pixelAspectRatio", &VideoProperties::getSar );
	addProperty( data, "displayAspectRatio", &VideoProperties::getDar );
	addProperty( data, "dtgActiveFormat", &VideoProperties::getDtgActiveFormat );
	addProperty( data, "colorTransfert", &VideoProperties::getColorTransfert );
	addProperty( data, "colorspace", &VideoProperties::getColorspace );
	addProperty( data, "colorRange", &VideoProperties::getColorRange );
	addProperty( data, "colorPrimaries", &VideoProperties::getColorPrimaries );
	addProperty( data, "chromaSampleLocation", &VideoProperties::getChromaSampleLocation );
	addProperty( data, "interlaced ", &VideoProperties::isInterlaced );
	addProperty( data, "topFieldFirst", &VideoProperties::isTopFieldFirst );
	addProperty( data, "fieldOrder", &VideoProperties::getFieldOrder );
	addProperty( data, "fps", &VideoProperties::getFps );
	addProperty( data, "nbFrame", &VideoProperties::getNbFrames );
	addProperty( data, "ticksPerFrame", &VideoProperties::getTicksPerFrame );
	addProperty( data, "bitRate", &VideoProperties::getBitRate );
	addProperty( data, "maxBitRate", &VideoProperties::getMaxBitRate );
	addProperty( data, "minBitRate", &VideoProperties::getMinBitRate );
	addProperty( data, "gopSize", &VideoProperties::getGopSize );

	std::string gop;
	for( size_t frameIndex = 0; frameIndex < _gopStructure.size(); ++frameIndex )
	{
		gop += _gopStructure.at( frameIndex ).first;
		gop += " ";
	}
	detail::add( data, "gop", gop );
	//detail::add( data, "isClosedGop", isClosedGop() );

	addProperty( data, "hasBFrames", &VideoProperties::hasBFrames );
	addProperty( data, "referencesFrames", &VideoProperties::getReferencesFrames );

	// Add properties of the pixel
	PropertyVector pixelProperties = _pixelProperties.getPropertiesAsVector();
	data.insert( data.end(), pixelProperties.begin(), pixelProperties.end() );

	return data;
}

}
