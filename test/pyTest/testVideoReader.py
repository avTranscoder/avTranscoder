import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_VIDEO_AVI_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables AVTRANSCODER_TEST_VIDEO_AVI_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testVideoReader():
    """
    Read a video stream with the VideoReader.
    The InputFile is created inside the reader.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    reader = av.VideoReader(av.InputStreamDesc(inputFileName, 0))

    # read all frames and check their size
    for i in xrange(0, reader.getSourceVideoProperties().getNbFrames()):
        frame = reader.readNextFrame()
        bytesPerPixel = reader.getOutputBitDepth() / 8
        assert_equals( frame.getDataSize(), reader.getOutputWidth() * reader.getOutputHeight() * bytesPerPixel )

    # check if there is no next frame
    assert_equals( reader.readNextFrame(), None )


def testVideoReaderWithGenerator():
    """
    Read a video stream with the VideoReader.
    When there is no more data to decode, switch to a generator and process some frames.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    reader = av.VideoReader(av.InputStreamDesc(inputFileName, 0))

    # read all frames and check their size
    for i in xrange(0, reader.getSourceVideoProperties().getNbFrames()):
        frame = reader.readNextFrame()
        bytesPerPixel = reader.getOutputBitDepth() / 8
        assert_equals( frame.getDataSize(), reader.getOutputWidth() * reader.getOutputHeight() * bytesPerPixel )

    # check if there is no next frame
    assert_equals( reader.readNextFrame(), None )

    # generate 10 frames of black
    reader.continueWithGenerator()
    for i in xrange(0, 9):
        frame = reader.readNextFrame()
        bytesPerPixel = reader.getOutputBitDepth() / 8
        assert_equals( frame.getDataSize(), reader.getOutputWidth() * reader.getOutputHeight() * bytesPerPixel )
