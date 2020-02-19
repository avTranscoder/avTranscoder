import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variable AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testNbSamplesAudioRewrapFromWav():
    """
    Rewrap one audio stream from WAV file, check nb samples.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testNbSamplesAudioRewrapFromWav.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.addStream( av.InputStreamDesc(inputFileName) )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file of rewrap
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_audioStream = src_properties.getAudioProperties()[0]

    # get dst file of rewrap
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    assert_equals( src_audioStream.getNbSamples(), dst_audioStream.getNbSamples() )
    assert_equals( src_audioStream.getNbSamples(), src_audioStream.getSampleRate() * src_audioStream.getDuration() )

def testNbSamplesAudioRewrapFromMov():
    """
    Rewrap one audio stream from MOV file, check nb samples.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testNbSamplesAudioRewrapFromMov.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.addStream( av.InputStreamDesc(inputFileName, 1) )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file of rewrap
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_audioStream = src_properties.getAudioProperties()[0]

    # get dst file of rewrap
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    assert_equals( src_audioStream.getNbSamples(), dst_audioStream.getNbSamples() )
    assert_equals( src_audioStream.getNbSamples(), src_audioStream.getSampleRate() * src_audioStream.getDuration() )

def testNbSamplesAudioTranscode():
    """
    Transcode one audio stream (to wave24b48kmono), check nb samples.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testNbSamplesAudioTranscode.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    # create custom profile
    customProfile = av.ProfileMap()
    customProfile[av.avProfileIdentificator] = "customProfile"
    customProfile[av.avProfileIdentificatorHuman] = "custom profile"
    customProfile[av.avProfileType] = av.avProfileTypeAudio
    customProfile[av.avProfileCodec] = "pcm_s16le"

    transcoder.addStream( av.InputStreamDesc(inputFileName), customProfile )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file of transcode
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_audioStream = src_properties.getAudioProperties()[0]

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    assert_equals( src_audioStream.getNbSamples(), dst_audioStream.getNbSamples() )
    assert_equals( src_audioStream.getNbSamples(), src_audioStream.getSampleRate() * src_audioStream.getDuration() )
