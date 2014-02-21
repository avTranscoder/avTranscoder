#ifndef _AV_TRANSCODER_VIDEO_STREAM_PROPERTIES_HPP_
#define _AV_TRANSCODER_VIDEO_STREAM_PROPERTIES_HPP_

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
}
#include <iostream>
#ifdef _MSC_VER
#include <float.h>
#define isnan _isnan
#define isinf(x) (!_finite(x))
#endif

namespace avtranscoder
{

namespace details
{

void getGopProperties( VideoProperties& vp, AVFormatContext* formatContext, AVCodecContext* codecContext, AVCodec* codec, const int index )
{
	AVPacket pkt;
	AVFrame* frame = avcodec_alloc_frame();
	
	av_init_packet( &pkt );
	avcodec_open2( codecContext, codec, NULL );

	int count = 0;
	int gotFrame = 0;
	
	while( ! av_read_frame( formatContext, &pkt ) )
	{
		if( pkt.stream_index == index )
		{
			avcodec_decode_video2( codecContext, frame, &gotFrame, &pkt );
			if( gotFrame )
			{
				vp.gopStructure.push_back( std::pair<char, bool>( av_get_picture_type_char( frame->pict_type ), frame->key_frame ) );
				vp.isInterlaced  = frame->interlaced_frame;
				vp.topFieldFirst = frame->top_field_first;
				++count;
			}
		}

		av_free_packet( &pkt );
		
		if( codecContext->gop_size == count )
		{
			break;
		}
	}

	av_frame_free( &frame );
}

}

VideoProperties videoStreamInfo( AVFormatContext* formatContext, const size_t index )
{
	VideoProperties vp;
	AVCodecContext* codec_context = formatContext->streams[index]->codec;
	
	codec_context->skip_frame = AVDISCARD_NONE;

	vp.streamId         = index;

	vp.codecName        = codec_context->codec_name;
	vp.codecLongName    = codec_context->codec_name;
	vp.bitRate          = codec_context->bit_rate;
	vp.isInterlaced     = false;

	vp.ticksPerFrame    = codec_context->ticks_per_frame,
	vp.width            = codec_context->width,
	vp.height           = codec_context->height,
	vp.gopSize          = codec_context->gop_size,
	vp.dtgActiveFormat  = codec_context->dtg_active_format,
	vp.referencesFrames = codec_context->refs,
	vp.profile          = codec_context->profile,
	vp.level            = codec_context->level;

	vp.timeBase.num     = codec_context->time_base.num;
	vp.timeBase.den     = codec_context->time_base.den;
	vp.sar.num          = codec_context->sample_aspect_ratio.num;
	vp.sar.den          = codec_context->sample_aspect_ratio.den;
	
	int darNum, darDen;
	av_reduce( &darNum, &darDen,
			   codec_context->width * codec_context->sample_aspect_ratio.num,
			   codec_context->height * codec_context->sample_aspect_ratio.den,
			   1024 * 1024);
	vp.dar.num = darNum;
	vp.dar.den = darDen;

	vp.fps = 1.0 * codec_context->time_base.den / ( codec_context->time_base.num * codec_context->ticks_per_frame );
	
	if( isinf( vp.fps ) )
		vp.fps = 0.0;

	vp.hasBFrames           = (bool) codec_context->has_b_frames;

	
	vp.colorspace           = "";
	vp.colorTransfert       = "";
	vp.colorRange           = "";
	vp.colorPrimaries       = "";
	vp.chromaSampleLocation = "";
	vp.fieldOrder           = "";
	
	switch( codec_context->color_trc )
	{
		case AVCOL_TRC_BT709:       vp.colorTransfert = "Rec 709"; break;
		case AVCOL_TRC_UNSPECIFIED: vp.colorTransfert = "unspecified"; break;
		case AVCOL_TRC_GAMMA22:     vp.colorTransfert = "Gamma 2.2"; break;
		case AVCOL_TRC_GAMMA28:     vp.colorTransfert = "Gamma 2.8"; break;
		case AVCOL_TRC_SMPTE240M:   vp.colorTransfert = "Smpte 240M"; break;
		case AVCOL_TRC_NB:          vp.colorTransfert = "Not ABI"; break;
	}
	switch( codec_context->colorspace )
	{
		case AVCOL_SPC_RGB:         vp.colorspace = "RGB"; break;
		case AVCOL_SPC_BT709:       vp.colorspace = "Rec 709"; break;
		case AVCOL_SPC_UNSPECIFIED: vp.colorspace = "unspecified"; break;
		case AVCOL_SPC_FCC:         vp.colorspace = "Four CC"; break;
		case AVCOL_SPC_BT470BG:     vp.colorspace = "BT470 (PAL - 625)"; break;
		case AVCOL_SPC_SMPTE170M:   vp.colorspace = "Smpte 170M (NTSC)"; break;
		case AVCOL_SPC_SMPTE240M:   vp.colorspace = "Smpte 240M"; break;
		case AVCOL_SPC_YCOCG:       vp.colorspace = "Y Co Cg"; break;
		case AVCOL_SPC_NB:          vp.colorspace = "Not ABI"; break;
	}
	switch( codec_context->color_range )
	{
		case AVCOL_RANGE_UNSPECIFIED: vp.colorRange = "unspecified"; break;
		case AVCOL_RANGE_MPEG:        vp.colorRange = "Head"; break;
		case AVCOL_RANGE_JPEG:        vp.colorRange = "Full"; break;
		case AVCOL_RANGE_NB:          vp.colorRange = "Not ABI"; break;
	}
	switch( codec_context->color_primaries )
	{
		case AVCOL_PRI_BT709:       vp.colorPrimaries = "Rec 709"; break;
		case AVCOL_PRI_UNSPECIFIED: vp.colorPrimaries = "unspecified"; break;
		case AVCOL_PRI_BT470M:      vp.colorPrimaries = "BT 470M"; break;
		case AVCOL_PRI_BT470BG:     vp.colorPrimaries = "Rec 601 (PAL & SECAM)"; break;
		case AVCOL_PRI_SMPTE170M:   vp.colorPrimaries = "Rec 601 (NTSC)"; break;
		case AVCOL_PRI_SMPTE240M:   vp.colorPrimaries = "Smpte 240 (NTSC)"; break;
		case AVCOL_PRI_FILM:        vp.colorPrimaries = "Film"; break;
		case AVCOL_PRI_NB:          vp.colorPrimaries = "Not ABI"; break;
	}
	switch( codec_context->chroma_sample_location )
	{
		case AVCHROMA_LOC_UNSPECIFIED: vp.chromaSampleLocation = "unspecified"; break;
		case AVCHROMA_LOC_LEFT:        vp.chromaSampleLocation = "left (mpeg2/4, h264 default)"; break;
		case AVCHROMA_LOC_CENTER:      vp.chromaSampleLocation = "center (mpeg1, jpeg, h263)"; break;
		case AVCHROMA_LOC_TOPLEFT:     vp.chromaSampleLocation = "top left"; break;
		case AVCHROMA_LOC_TOP:         vp.chromaSampleLocation = "top"; break;
		case AVCHROMA_LOC_BOTTOMLEFT:  vp.chromaSampleLocation = "bottom left"; break;
		case AVCHROMA_LOC_BOTTOM:      vp.chromaSampleLocation = "bottom"; break;
		case AVCHROMA_LOC_NB:          vp.chromaSampleLocation = "Not ABI"; break;
	}
	switch( codec_context->field_order )
	{
		case AV_FIELD_UNKNOWN:     vp.fieldOrder = "unknown"; break;
		case AV_FIELD_PROGRESSIVE: vp.fieldOrder = "progressive"; break;
		case AV_FIELD_TT:          vp.fieldOrder = "top top"; break;
		case AV_FIELD_BB:          vp.fieldOrder = "bottom bottom"; break;
		case AV_FIELD_TB:          vp.fieldOrder = "top bottom"; break;
		case AV_FIELD_BT:          vp.fieldOrder = "bottom top"; break;
	}
	
	const AVPixFmtDescriptor* pixFmt = av_pix_fmt_desc_get( codec_context->pix_fmt );
	if( pixFmt != NULL )
	{
		vp.pixelName       = pixFmt->name;
		vp.componentsCount = pixFmt->nb_components;
		vp.chromaWidth     = pixFmt->log2_chroma_w;
		vp.chromaHeight    = pixFmt->log2_chroma_h;
		vp.endianess       = ( pixFmt->flags & PIX_FMT_BE ) ? "big" : "little";
		vp.indexedColors        = (bool) pixFmt->flags & PIX_FMT_PAL;
		vp.bitWisePacked        = (bool) pixFmt->flags & PIX_FMT_BITSTREAM;
		vp.hardwareAcceleration = (bool) pixFmt->flags & PIX_FMT_HWACCEL;
		vp.notFirstPlane        = (bool) pixFmt->flags & PIX_FMT_PLANAR;
		vp.rgbPixelData         = (bool) pixFmt->flags & PIX_FMT_RGB;
		vp.pseudoPaletted       = (bool) pixFmt->flags & PIX_FMT_PSEUDOPAL;
		vp.asAlpha              = (bool) pixFmt->flags & PIX_FMT_ALPHA;

		for( size_t channel = 0; channel < (size_t)pixFmt->nb_components; ++channel )
		{
			Channel c;
			c.id           = channel;
			c.chromaHeight = (size_t)pixFmt->comp[channel].plane;
			c.bitStep      = (size_t)pixFmt->comp[channel].step_minus1;
			vp.channels.push_back( c );
		}
	}

	AVCodec* codec = NULL;
	if( ( codec = avcodec_find_decoder( codec_context->codec_id ) ) != NULL )
	{
		if( codec->capabilities & CODEC_CAP_TRUNCATED )
			codec_context->flags|= CODEC_FLAG_TRUNCATED;

		vp.codecName     = codec->name;
		vp.codecLongName = codec->long_name;

		if( codec_context->profile == -99 )
		{
			const char* profile;
			if( ( profile = av_get_profile_name( codec, codec_context->profile ) ) != NULL )
				vp.profileName = profile;
		}
		
		if( codec_context->width && codec_context->height )
		{
			details::getGopProperties( vp, formatContext, codec_context, codec, index);
		}
	}

	return vp;
}

}

#endif