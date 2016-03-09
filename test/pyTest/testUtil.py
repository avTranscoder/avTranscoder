from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testSupportedPixelFormats():
    """
    Check supported pixel formats of mpeg2video codec.
    Check access to the list of all supported pixel formats.
    """
    # Check supported pixel formats of mpeg2video codec
    videoCodecName = "mpeg2video"
    mpeg2videoSupportedPixelFormats = av.getSupportedPixelFormats(videoCodecName)
    assert_equals(len(mpeg2videoSupportedPixelFormats), 2)
    assert_in("yuv420p", mpeg2videoSupportedPixelFormats)
    assert_in("yuv422p", mpeg2videoSupportedPixelFormats)
    
    # Check if there is more pixel formats available
    allSupportedPixelFormats = av.getSupportedPixelFormats()
    assert_less(len(mpeg2videoSupportedPixelFormats), len(allSupportedPixelFormats))


def testSupportedSampleFormats():
    """
    Check supported sample formats of pcm_s24le codec.
    Check access to the list of all supported sample formats.
    """
    # Check supported sample formats of pcm_s24le codec
    audioCodecName = "pcm_s24le"
    pcmSupportedSampleFormats = av.getSupportedSampleFormats(audioCodecName)
    assert_equals(len(pcmSupportedSampleFormats), 1)
    assert_in("s32", pcmSupportedSampleFormats)
    
    # Check if there is more sample formats available
    allSupportedSampleFormats = av.getSupportedSampleFormats()
    assert_less(len(pcmSupportedSampleFormats), len(allSupportedSampleFormats))


def testAccessToAPixelFormat():
    """
    Check if we can access an AVPixelFormat from a name, and retrieve the name from this constant value.
    """
    strPixelFormat = "yuv420p"
    avPixelFormat = av.getAVPixelFormat(strPixelFormat)
    assert_equals(av.getPixelFormatName(avPixelFormat), strPixelFormat)
    

def testAccessToASampleFormat():
    """
    Check if we can access an AVSampleFormat from a name, and retrieve the name from this constant value.
    """
    strSampleFormat = "s32"
    avSampleFormat = av.getAVSampleFormat(strSampleFormat)
    assert_equals(av.getSampleFormatName(avSampleFormat), strSampleFormat)


def testAccessToAvailableFormatsNames():
    """
    Check if we can access the list of available formats.
    @note Tested with ffmpeg-2.4.2
    """
    availableFormats = av.getAvailableFormatsNames()
    assert_greater_equal(len(availableFormats), 135)


def testAccessToAvailableVideoCodecsNames():
    """
    Check if we can access the list of available video codecs.
    @note Tested with ffmpeg-2.4.2
    """
    availableVideoCodecs = av.getAvailableVideoCodecsNames()
    assert_greater_equal(len(availableVideoCodecs), 204)


def testAccessToAvailableAudioCodecsNames():
    """
    Check if we can access the list of available audio codecs.
    @note Tested with ffmpeg-2.4.2
    """
    availableAudioCodecs = av.getAvailableAudioCodecsNames()
    assert_greater_equal(len(availableAudioCodecs), 152)
