import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testAudioReader():
    """
    Read a audio stream with the AudioReader.
    The InputFile is created inside the reader.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    reader = av.AudioReader(av.InputStreamDesc(inputFileName))

    # read all frames and check their size
    while True:
        frame = reader.readNextFrame()
        if not frame:
            break
        assert_greater(frame.getDataSize(), 0)

    # check if there is no next frame
    frame = reader.readNextFrame()
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
    readerOfAllChannels = av.AudioReader(av.InputStreamDesc(inputFileName, streamIndex))
    nbChannels = readerOfAllChannels.getOutputNbChannels()
    # read first frame
    frame = readerOfAllChannels.readNextFrame()
    sizeOfFrameWithAllChannels = frame.getDataSize()

    # create reader to read one channel of the audio stream
    readerOfOneChannel = av.AudioReader(av.InputStreamDesc(inputFileName, streamIndex, channelIndex))
    # read first frame
    frame = readerOfOneChannel.readNextFrame()
    sizeOfFrameWithOneChannels = frame.getDataSize()

    assert_equals( sizeOfFrameWithAllChannels / nbChannels, sizeOfFrameWithOneChannels )


def testAudioReaderWithGenerator():
    """
    Read an audio stream with the AudioReader.
    When there is no more data to decode, switch to a generator and process some frames.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    reader = av.AudioReader(av.InputStreamDesc(inputFileName))

    # read all frames and check their size
    while True:
        frame = reader.readNextFrame()
        if not frame:
            break
        assert_greater(frame.getDataSize(), 0)

    # check if there is no next frame
    assert_equals( reader.readNextFrame(), None )

    # generate 10 frames of silence
    reader.continueWithGenerator()
    for i in range(0, 9):
        frame = reader.readNextFrame()
        # assuming we generate data of 1920 samples of 2 bytes
        nbSamplesPerChannel = 1920
        bytesPerSample = 2
        assert_equals(frame.getDataSize(), reader.getOutputNbChannels() * nbSamplesPerChannel * bytesPerSample )
