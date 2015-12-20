import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_AUDIO_MOV_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_AUDIO_WAVE_FILE and "
        "AVTRANSCODER_TEST_AUDIO_MOV_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testAddStreamTranscoder():
    """
    Add a streamTranscoder to the Transcoder, and process rewrap of an audio stream.
    """
    # input
    inputFile = av.InputFile( os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE'] )
    inputIndex = 0
    inputFile.activateStream( inputIndex )

    # output
    outputFileName = "testAddStreamTranscoder.avi"
    ouputFile = av.OutputFile( outputFileName )

    streamTranscoder = av.StreamTranscoder( inputFile.getStream( inputIndex ), ouputFile )
    transcoder = av.Transcoder( ouputFile )
    transcoder.add( streamTranscoder)

    # process
    progress = av.NoDisplayProgress()
    transcoder.process( progress )


@raises(IOError)
def testAddAllStreamsOfFileWhichDoesNotExist():
    """
    Add all streams from a given file.
    """
    # input
    inputFileName = "fileWhichDoesNotExist.mov"

    # output
    outputFileName = "testAddAllStreamsOfFileWhichDoesNotExist.mov"
    ouputFile = av.OutputFile( outputFileName )

    transcoder = av.Transcoder( ouputFile )
    transcoder.add( inputFileName )

    # process
    progress = av.ConsoleProgress()
    transcoder.process( progress )


def testAddAllStreamsOfAGivenFile():
    """
    Add all streams from a given file.
    """
    # input
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']

    # output
    outputFileName = "testAddAllStreamsOfAGivenFile.mov"
    ouputFile = av.OutputFile( outputFileName )

    transcoder = av.Transcoder( ouputFile )
    transcoder.add( inputFileName )

    # process
    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_streams_properties = src_properties.getStreamProperties()

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_streams_properties = dst_properties.getStreamProperties()

    import testTranscoderRewrap
    # for each stream
    for src_stream, dst_stream in zip(src_streams_properties, dst_streams_properties):
        # check properties
        testTranscoderRewrap.checkStream(src_stream, dst_stream)
