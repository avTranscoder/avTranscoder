import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variable AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testTranscodeWave24b48k5_1():
    """
    Transcode one audio stream (profile wave24b48k5_1).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testTranscodeWave24b48k5_1.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_audioStream = inputFile.getProperties().getAudioProperties()[0]
    audioStreamIndex = src_audioStream.getStreamIndex()
    transcoder.add( inputFileName, audioStreamIndex, "wave24b48k5_1" )

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
    assert_equals( "s32", dst_audioStream.getSampleFormatName() )
    assert_equals( "signed 32 bits", dst_audioStream.getSampleFormatLongName() )
    assert_equals( 48000, dst_audioStream.getSampleRate() )
    assert_equals( 6, dst_audioStream.getNbChannels() )

def testTranscodeWave24b48kstereo():
    """
    Transcode one audio stream (profile wave24b48kstereo).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testTranscodeWave24b48kstereo.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_audioStream = inputFile.getProperties().getAudioProperties()[0]
    audioStreamIndex = src_audioStream.getStreamIndex()
    transcoder.add( inputFileName, audioStreamIndex, "wave24b48kstereo" )

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
    assert_equals( "s32", dst_audioStream.getSampleFormatName() )
    assert_equals( "signed 32 bits", dst_audioStream.getSampleFormatLongName() )
    assert_equals( 48000, dst_audioStream.getSampleRate() )
    assert_equals( 2, dst_audioStream.getNbChannels() )

def testTranscodeWave24b48kmono():
    """
    Transcode one audio stream (profile wave24b48kmono).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testTranscodeWave24b48kmono.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_audioStream = inputFile.getProperties().getAudioProperties()[0]
    audioStreamIndex = src_audioStream.getStreamIndex()
    transcoder.add( inputFileName, audioStreamIndex, "wave24b48kmono" )

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
    assert_equals( "s32", dst_audioStream.getSampleFormatName() )
    assert_equals( "signed 32 bits", dst_audioStream.getSampleFormatLongName() )
    assert_equals( 48000, dst_audioStream.getSampleRate() )
    assert_equals( 1, dst_audioStream.getNbChannels() )

def testTranscodeWave16b48kmono():
    """
    Transcode one audio stream (profile wave16b48kmono).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testTranscodeWave16b48kmono.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_audioStream = inputFile.getProperties().getAudioProperties()[0]
    audioStreamIndex = src_audioStream.getStreamIndex()
    transcoder.add( inputFileName, audioStreamIndex, "wave16b48kmono" )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    audioStat = processStat.getAudioStat(0)
    assert_equals(src_audioStream.getDuration(), audioStat.getDuration())

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    assert_equals( "pcm_s16le", dst_audioStream.getCodecName() )
    assert_equals( "s16", dst_audioStream.getSampleFormatName() )
    assert_equals( "signed 16 bits", dst_audioStream.getSampleFormatLongName() )
    assert_equals( 48000, dst_audioStream.getSampleRate() )
    assert_equals( 1, dst_audioStream.getNbChannels() )
