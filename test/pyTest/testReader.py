import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_VIDEO_AVI_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_VIDEO_AVI_FILE and "
        "AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testVideoReaderCreateNewInputFile():
    """
    Read a video stream with the VideoReader.
    The InputFile is created inside the reader.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    reader = av.VideoReader(inputFileName)

    # read all frames and check their size
    for i in xrange(0, reader.getSourceVideoProperties().getNbFrames()):
        frame = av.VideoFrame(reader.readNextFrame())
        bytesPerPixel = reader.getOutputBitDepth() / 8
        assert_equals( frame.getSize(), reader.getOutputWidth() * reader.getOutputHeight() * bytesPerPixel )

    # check if there is no next frame
    frame = reader.readNextFrame()
    assert_equals( reader.readNextFrame(), None )


def testVideoReaderReferenceInputFile():
    """
    Read a video stream with the VideoReader.
    The InputFile is a reference for the reader.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    inputFile = av.InputFile(inputFileName)
    reader = av.VideoReader(inputFile)

    # read all frames and check their size
    for i in xrange(0, reader.getSourceVideoProperties().getNbFrames()):
        frame = av.VideoFrame(reader.readNextFrame())
        bytesPerPixel = reader.getOutputBitDepth() / 8
        assert_equals( frame.getSize(), reader.getOutputWidth() * reader.getOutputHeight() * bytesPerPixel )

    # check if there is no next frame
    assert_equals( reader.readNextFrame(), None )


def testAudioReaderChannelsExtraction():
    """
    Read the same audio stream with several AudioReaders.
    Compare decoded frames from reader of all channels, and of one channel.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    inputFile = av.InputFile(inputFileName)
    streamIndex = inputFile.getProperties().getAudioProperties()[0].getStreamIndex()
    channelIndex = 0

    # create reader to read all channels of the audio stream
    readerOfAllChannels = av.AudioReader(inputFile, streamIndex)
    nbChannels = readerOfAllChannels.getOutputNbChannels()
    # read first frame
    frame = av.AudioFrame(readerOfAllChannels.readNextFrame())
    sizeOfFrameWithAllChannels = frame.getSize()

    # create reader to read one channel of the audio stream
    readerOfOneChannel = av.AudioReader(inputFile, streamIndex, channelIndex)
    # read first frame
    frame = av.AudioFrame(readerOfOneChannel.readNextFrame())
    sizeOfFrameWithOneChannels = frame.getSize()

    assert_equals( sizeOfFrameWithAllChannels / nbChannels, sizeOfFrameWithOneChannels )


def testVideoReaderWithGenerator():
    """
    Read a video stream with the VideoReader.
    When there is no more data to decode, switch to a generator and process some frames.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    reader = av.VideoReader(inputFileName)

    # read all frames and check their size
    for i in xrange(0, reader.getSourceVideoProperties().getNbFrames()):
        frame = av.VideoFrame(reader.readNextFrame())
        bytesPerPixel = reader.getOutputBitDepth() / 8
        assert_equals( frame.getSize(), reader.getOutputWidth() * reader.getOutputHeight() * bytesPerPixel )

    # check if there is no next frame
    assert_equals( reader.readNextFrame(), None )

    # generate 10 frames of black
    reader.continueWithGenerator()
    for i in xrange(0, 9):
        frame = av.VideoFrame(reader.readNextFrame())
        bytesPerPixel = reader.getOutputBitDepth() / 8
        assert_equals( frame.getSize(), reader.getOutputWidth() * reader.getOutputHeight() * bytesPerPixel )


def testAudioReaderWithGenerator():
    """
    Read an audio stream with the AudioReader.
    When there is no more data to decode, switch to a generator and process some frames.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    inputFile = av.InputFile(inputFileName)
    reader = av.AudioReader(inputFile)

    # read all frames and check their size
    while True:
        frame = reader.readNextFrame()
        if not frame:
            break
        frame = av.AudioFrame(frame)
        nbSamplesPerChannel = frame.getNbSamplesPerChannel()
        bytesPerSample = 2
        assert_equals( frame.getSize(), reader.getOutputNbChannels() * nbSamplesPerChannel * bytesPerSample )

    # check if there is no next frame
    assert_equals( reader.readNextFrame(), None )

    # generate 10 frames of silence
    reader.continueWithGenerator()
    for i in xrange(0, 9):
        frame = av.AudioFrame(reader.readNextFrame())
        nbSamplesPerChannel = frame.getNbSamplesPerChannel()
        bytesPerSample = 2
        assert_equals( frame.getSize(), reader.getOutputNbChannels() * nbSamplesPerChannel * bytesPerSample )
