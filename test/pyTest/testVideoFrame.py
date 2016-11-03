from nose.tools import *

from pyAvTranscoder import avtranscoder as av


@raises(MemoryError)
def testInvalidVideoFrameAutoAllocated():
    """
    Try to create an invalid VideoFrame automatically allocated.
    """
    desc = av.VideoFrameDesc(1920, 1080, "toto")
    av.VideoFrame(desc)


@raises(MemoryError)
def testInvalidVideoFrameManualAllocated():
    """
    Try to create an invalid VideoFrame manually allocated.
    """
    width = 1920
    height = 1080
    pixelFormat = "titi"
    desc = av.VideoFrameDesc(width, height, pixelFormat)
    frame = av.VideoFrame(desc, False)

    assert_equals(frame.isDataAllocated(), False)
    assert_equals(frame.getDataSize(), 0)
    assert_equals(frame.getWidth(), width)
    assert_equals(frame.getHeight(), height)
    assert_equals(av.getPixelFormatName(frame.getPixelFormat()), "")

    frame.allocateData()

def testVideoFrame():
    """
    Check the size and the data buffer of a VideoFrame.
    """
    width = 1920
    height = 1080
    pixelFormat = "yuv420p"
    desc = av.VideoFrameDesc(width, height, pixelFormat)
    frame = av.VideoFrame(desc)

    assert_equals(frame.isDataAllocated(), True)
    assert_equals(frame.isVideoFrame(), True)
    assert_equals(frame.getDataSize(), width * height * 1.5)

    frame.freeData()
    assert_equals(frame.isDataAllocated(), False)
