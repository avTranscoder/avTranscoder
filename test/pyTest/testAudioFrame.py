from nose.tools import *

from pyAvTranscoder import avtranscoder as av


@raises(MemoryError)
def testInvalidAudioFrameAutoAllocated():
    """
    Try to create an invalid AudioFrame automatically allocated.
    """
    desc = av.AudioFrameDesc(4800, 1, "toto")
    av.AudioFrame(desc)


@raises(MemoryError)
def testInvalidAudioFrameManualAllocated():
    """
    Try to create an invalid AudioFrame manually allocated.
    """
    sampleRate = 48000
    nbChannels = 1
    sampleFormat = "titi"
    desc = av.AudioFrameDesc(sampleRate, nbChannels, sampleFormat)
    frame = av.AudioFrame(desc, False)

    assert_equals(frame.isDataAllocated(), False)
    assert_equals(frame.getSize(), 0)
    assert_equals(frame.getSampleRate(), sampleRate)
    assert_equals(frame.getNbChannels(), nbChannels)
    assert_equals(frame.getChannelLayoutDesc(), "mono")
    assert_equals(frame.getNbSamplesPerChannel(), 1920)
    assert_equals(frame.getBytesPerSample(), 0)
    assert_equals(av.getSampleFormatName(frame.getSampleFormat()), "")

    frame.allocateData()

def testAudioFrame():
    """
    Check the size and the data buffer of a AudioFrame.
    """
    sampleRate = 48000
    nbChannels = 1
    sampleFormat = "s32"
    desc = av.AudioFrameDesc(sampleRate, nbChannels, sampleFormat)
    frame = av.AudioFrame(desc)

    assert_equals(frame.isDataAllocated(), True)
    assert_equals(frame.isAudioFrame(), True)
    assert_equals(frame.getSize(), frame.getNbSamplesPerChannel() * frame.getBytesPerSample() * nbChannels)

    frame.freeData()
    assert_equals(frame.isDataAllocated(), False)
