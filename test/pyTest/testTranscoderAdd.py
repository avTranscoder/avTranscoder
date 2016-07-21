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
    transcoder.addStream( streamTranscoder)

    # process
    transcoder.process()


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
    transcoder.addStream( av.InputStreamDesc(inputFileName, 0) )

    # process
    transcoder.process()


def testAddSeveralInputsToCreateOneOutput():
    """
    Add several audio inputs and create one output stream.
    """
    # inputs
    inputs = av.InputStreamDescVector()
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    inputFile = av.InputFile(inputFileName)
    src_audioStream = inputFile.getProperties().getAudioProperties()[0]
    src_audioStreamIndex = src_audioStream.getStreamIndex()
    inputs.append(av.InputStreamDesc(inputFileName, src_audioStreamIndex, (0,1)))
    inputs.append(av.InputStreamDesc(inputFileName, src_audioStreamIndex, (2,3)))
    inputs.append(av.InputStreamDesc(inputFileName, src_audioStreamIndex, (4,5)))

    # output
    outputFileName = "testAddSeveralInputsToCreateOneOutput.mov"
    ouputFile = av.OutputFile(outputFileName)

    transcoder = av.Transcoder(ouputFile)
    transcoder.addStream(inputs)

    # process
    processStat = transcoder.process()

    # check process stat returned
    audioStat = processStat.getAudioStat(0)
    assert_equals(src_audioStream.getDuration(), audioStat.getDuration())

    # get dst file of transcode
    dst_inputFile = av.InputFile(outputFileName)
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    assert_equals( src_audioStream.getCodecName(), dst_audioStream.getCodecName() )
    assert_equals( src_audioStream.getSampleFormatName(), dst_audioStream.getSampleFormatName() )
    assert_equals( src_audioStream.getSampleFormatLongName(), dst_audioStream.getSampleFormatLongName() )
    assert_equals( src_audioStream.getSampleRate(), dst_audioStream.getSampleRate() )
    assert_equals( src_audioStream.getNbChannels(), dst_audioStream.getNbChannels() )
