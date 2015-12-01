import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_VIDEO_MP4_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_AUDIO_WAVE_FILE and "
        "AVTRANSCODER_TEST_VIDEO_MP4_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testAddMetadataDate():
    """
    Add metadata 'date' to the outputFile.
    """
    outputFileName = "testAddMetadataDate.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    # rewrap a stream
    transcoder.add( os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE'], 0, "")

    # add one metadata
    metadata_to_check = ("date", "value")
    ouputFile.addMetadata( metadata_to_check[0], metadata_to_check[1] )

    progress = av.NoDisplayProgress()
    transcoder.process( progress )

    inputFile = av.InputFile( outputFileName )
    inputFile.analyse( progress, av.eAnalyseLevelHeader )
    properties = inputFile.getProperties()

    assert_in( metadata_to_check, properties.getMetadatas() )

def testAddImpossibleMetadata():
    """
    Can't add an impossible metadata to the outputFile.
    """
    outputFileName = "testAddMetadataPlop.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    # rewrap a stream
    transcoder.add( os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE'], 0, "")

    # add one metadata
    metadata_to_check = ("undefinedMetadataKey", "undefinedMetadataValue")
    ouputFile.addMetadata( metadata_to_check[0], metadata_to_check[1] )

    progress = av.NoDisplayProgress()
    transcoder.process( progress )

    inputFile = av.InputFile( outputFileName )
    inputFile.analyse( progress, av.eAnalyseLevelHeader )
    properties = inputFile.getProperties()

    assert_not_in( metadata_to_check, properties.getMetadatas() )


def testCheckVideoProperties():
    """
    Check properties of a video stream.
    """
    # get src file
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MP4_FILE']
    inputFile = av.InputFile( inputFileName )
    properties = inputFile.getProperties()
    videoStream = properties.getVideoProperties()[0]

    expectedTotalBitRate = 3249739
    expectedVideoBitRate = 3247981

    expectedCodecName = 'h264'
    expectedWidth = 1920
    expectedHeight = 1080
    expectedNbFrames = 241
    expectedDuration = 10.04
    expectedFps = 24

    assert_equals( properties.getBitRate(), expectedTotalBitRate )
    assert_equals( videoStream.getBitRate(), expectedVideoBitRate )

    assert_equals( videoStream.getCodecName(), expectedCodecName )
    assert_equals( videoStream.getWidth(), expectedWidth )
    assert_equals( videoStream.getHeight(), expectedHeight )
    assert_equals( videoStream.getNbFrames(), expectedNbFrames )
    assert_equals( round(videoStream.getDuration(), 2), expectedDuration )
    assert_equals( videoStream.getFps(), expectedFps )

def testCheckAudioProperties():
    """
    Check properties of an audio stream.
    """
    # get src file
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    inputFile = av.InputFile( inputFileName )
    properties = inputFile.getProperties()
    audioStream = properties.getAudioProperties()[0]

    expectedTotalBitRate = 4608040
    expectedAudioBitRate = 4608000

    expectedCodecName = 'pcm_s16le'
    expectedSamples = 5760000
    expectedDuration = 20
    expectedChannels = 6
    expectedChannelLayout = '5.1'
    expectedSampleRate = 48000

    assert_equals( properties.getBitRate(), expectedTotalBitRate )
    assert_equals( audioStream.getBitRate(), expectedAudioBitRate )

    assert_equals( audioStream.getCodecName(), expectedCodecName )
    assert_equals( audioStream.getNbSamples(), expectedSamples )
    assert_equals( round(audioStream.getDuration(), 2), expectedDuration )
    assert_equals( audioStream.getNbChannels(), expectedChannels )
    assert_equals( audioStream.getChannelLayout(), expectedChannelLayout )
    assert_equals( audioStream.getSampleRate(), expectedSampleRate )