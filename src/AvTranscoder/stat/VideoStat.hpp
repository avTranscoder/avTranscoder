#ifndef _AV_TRANSCODER_VIDEOSTAT_HPP
#define _AV_TRANSCODER_VIDEOSTAT_HPP

#include <AvTranscoder/common.hpp>

namespace avtranscoder
{

/**
 * @brief Statistics related to a video stream.
 */
class AvExport VideoStat
{
public:
    VideoStat(const float duration, const size_t nbFrames)
        : _duration(duration)
        , _nbFrames(nbFrames)
        , _quality(0)
        , _psnr(0)
    {
    }

public:
    float getDuration() const { return _duration; }
    size_t getNbFrames() const { return _nbFrames; }
    size_t getQuality() const { return _quality; }
    double getPSNR() const { return _psnr; }

    void setQuality(const size_t quality) { _quality = quality; }
    void setPSNR(const double mse) { _psnr = VideoStat::toPSNR(mse); }

private:
    static double toPSNR(const double mse);

private:
    float _duration;
    size_t _nbFrames;
    size_t _quality; ///< Between 1 (good) and FF_LAMBDA_MAX (bad). 0 if unknown.
    double _psnr;    ///< 0 if unknown.
};
}

#endif
