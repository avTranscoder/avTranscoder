#ifndef _AVTRANSCODER_PROFIL_XDCAMHD422_HPP_
#define _AVTRANSCODER_PROFIL_XDCAMHD422_HPP_

namespace avtranscoder
{

void loadXdCamHD422( Profile::ProfilesDesc& profiles )
{
	Profile::ProfileDesc xdCamHd422;

	xdCamHd422[ Profile::avProfilIdentificator ] = "xdcamhd422";
	xdCamHd422[ Profile::avProfilIdentificatorHuman ] = "XdCamHD 422";


	xdCamHd422[ "codec" ] = "mpeg2video";
	xdCamHd422[ "profile" ] = "0"; // FF_PROFILE_MPEG2_422
	xdCamHd422[ "level" ] = "2";
	xdCamHd422[ "g" ] = "12";
	xdCamHd422[ "bf" ] = "2"; // max_b_frames
	xdCamHd422[ "dtg_active_format" ] = "10"; // FF_DTG_AFD_16_9
	xdCamHd422[ "qmin" ] = "1";
	xdCamHd422[ "dc" ] = "2";

	xdCamHd422[ "pix_fmt" ] = "yuv422p";


	// color informations are not used in FFmpeg/LibAV for Mpeg2
	// xdCamHd422[ "colorspace" ] = "AVCOL_SPC_BT709";
	// xdCamHd422[ "color_trc" ] = "AVCOL_SPC_BT709";
	// xdCamHd422[ "color_primaries" ] = "AVCOL_SPC_BT709";
	// xdCamHd422[ "color_range" ] = "AVCOL_RANGE_MPEG";

	xdCamHd422[ "timecode_frame_start" ] = "900000"; // 10:00:00:00

	xdCamHd422[ "b" ] = "50000000";
	xdCamHd422[ "minrate" ] = "50000000";
	xdCamHd422[ "maxrate" ] = "50000000";

	xdCamHd422[ "bufsize" ] = "50000000";
	xdCamHd422[ "rc_max_vbv_use" ] = "1";
	xdCamHd422[ "rc_min_vbv_use" ] = "1";
	xdCamHd422[ "lmin" ] = "1*QP2LAMBDA";
	xdCamHd422[ "field_order" ] = "tb";

	// research modes
	xdCamHd422[ "me_method" ] = "dia";
	xdCamHd422[ "me_range" ] = "0"; // no limit !
	xdCamHd422[ "dia_size" ] = "0";
	xdCamHd422[ "pre_dia_size" ] = "0";
	xdCamHd422[ "mbd" ] = "rd"; //mb_decision = Rate-Distorsion // FF_MB_DECISION_RD

	xdCamHd422[ "ildctcmp" ] = "vsse"; // FF_CMP_VSSE
	xdCamHd422[ "cmp" ] = "sse"; // me_cmp
	xdCamHd422[ "subcmp" ] = "sse"; // me_sub_cmp
	xdCamHd422[ "precmp" ] = "sse"; // me_pre_cmp

	xdCamHd422[ "rc_max_vbv_use" ] = "1";
	xdCamHd422[ "rc_min_vbv_use" ] = "1";

	xdCamHd422[ "global_quality" ] = "1*QP2LAMBDA"; // 1 * FF_QP2LAMBDA

	xdCamHd422[ "flags" ] = "cgop"; // open GOP

	// xdCamHd422[ "intra_vlc" ] = "1";
	// xdCamHd422[ "non_linear_quant" ] = "1";

	xdCamHd422[ "threads" ] = "6";

	profiles.push_back( xdCamHd422 );
}

}

#endif