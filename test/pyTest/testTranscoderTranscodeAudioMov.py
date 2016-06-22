import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_MOV_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variable AVTRANSCODER_TEST_AUDIO_MOV_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testTranscodeMovVariableNbSamplesPerFrame():
    """
    Transcode the audio stream of a MOV file which contains a video stream.
    The number of samples per frame can vary to fit the gop size.
    AudioTransform must manage these cases.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testTranscodeMovVariableNbSamplesPerFrame.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    # create custom profile
    customProfile = av.ProfileMap()
    customProfile[av.avProfileIdentificator] = "customProfile"
    customProfile[av.avProfileIdentificatorHuman] = "custom profile"
    customProfile[av.avProfileType] = av.avProfileTypeAudio
    customProfile[av.avProfileCodec] = "pcm_s24le"

    inputFile = av.InputFile( inputFileName )
    src_audioStream = inputFile.getProperties().getAudioProperties()[0]
    audioStreamIndex = src_audioStream.getStreamIndex()
    transcoder.add( inputFileName, audioStreamIndex, customProfile )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    audioStat = processStat.getAudioStat(0)
    assert_equals(src_audioStream.getDuration(), audioStat.getDuration())

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    assert_equals( "pcm_s24le", dst_audioStream.getCodecName() )


def testTranscodeMovExtractChannels():
    """
    Transcode the audio stream of a MOV file which contains a video stream.
    Extract channel one and third of the audio stream (5.1).
    The encoding profile will be found from from input.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testTranscodeMovExtractChannels.mov"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_audioStream = inputFile.getProperties().getAudioProperties()[0]
    audioStreamIndex = src_audioStream.getStreamIndex()
    transcoder.add( inputFileName, audioStreamIndex, 0 )
    transcoder.add( inputFileName, audioStreamIndex, 3 )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    audioStat = processStat.getAudioStat(0)
    assert_equals(src_audioStream.getDuration(), audioStat.getDuration())

    # check dst audio streams
    dst_inputFile = av.InputFile( outputFileName )
    for dst_audioStream in dst_inputFile.getProperties().getAudioProperties():
        assert_equals( 1, dst_audioStream.getNbChannels() )
