import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None or os.environ.get('AVTRANSCODER_TEST_AUDIO_MOV_FILE') is None or os.environ.get('AVTRANSCODER_TEST_VIDEO_AVI_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables AVTRANSCODER_TEST_VIDEO_AVI_FILE / AVTRANSCODER_TEST_AUDIO_MOV_FILE / AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testTranscodeAudioPositiveOffset():
    """
    Transcode one audio stream (profile wave24b48kmono) with offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testTranscodeAudioPositiveOffset.wav"
    offset = 10

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "wave24b48kmono", offset )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_audioStream = src_properties.getAudioProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    # check output duration
    assert_equals( src_audioStream.getDuration() + offset, dst_audioStream.getDuration() )


def testTranscodeAudioNegativeOffset():
    """
    Transcode one audio stream (profile wave24b48kmono) with a negative offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testTranscodeAudioNegativeOffset.wav"
    offset = -5.5

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "wave24b48kmono", offset )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_audioStream = src_properties.getAudioProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    # check output duration
    assert_equals( src_audioStream.getDuration() + offset, dst_audioStream.getDuration() )


def testRewrapAudioPositiveOffset():
    """
    Rewrap one audio stream with offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testRewrapAudioPositiveOffset.wav"
    offset = 10

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "", offset )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_audioStream = src_properties.getAudioProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    # check output duration
    assert_equals( src_audioStream.getDuration() + offset, dst_audioStream.getDuration() )
    assert_equals( src_audioStream.getNbSamples() + ( offset * dst_audioStream.getSampleRate() * dst_audioStream.getChannels() ), dst_audioStream.getNbSamples() )


def testRewrapAudioNegativeOffset():
    """
    Rewrap one audio stream with a negative offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testRewrapAudioNegativeOffset.wav"
    offset = -5.5

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "", offset )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_audioStream = src_properties.getAudioProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    # check output duration
    assert_equals( src_audioStream.getDuration() + offset, dst_audioStream.getDuration() )
    assert_equals( src_audioStream.getNbSamples() + ( offset * dst_audioStream.getSampleRate() * dst_audioStream.getChannels() ), dst_audioStream.getNbSamples() )


def testTranscodeVideoPositiveOffset():
    """
    Transcode one video stream (profile mpeg2) with offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testTranscodeVideoPositiveOffset.mov"
    offset = 10

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "mpeg2", offset )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

     # check output duration
    assert_equals( src_videoStream.getDuration() + offset, dst_videoStream.getDuration() )


def testTranscodeVideoNegativeOffset():
    """
    Transcode one video stream (profile mpeg2) with a negative offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testTranscodeVideoNegativeOffset.mov"
    offset = -5.5

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "mpeg2", offset )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    # check output duration
    assert_equals( src_videoStream.getDuration() + offset, dst_videoStream.getDuration() )


def testRewrapVideoPositiveOffset():
    """
    Rewrap one video stream with offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testRewrapVideoPositiveOffset.mov"
    offset = 10

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "", offset )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    # check output duration
    assert_equals( src_videoStream.getDuration() + offset, dst_videoStream.getDuration() )
    assert_equals( src_videoStream.getNbFrames() + ( offset * dst_videoStream.getFps() ), dst_videoStream.getNbFrames() )


def testRewrapVideoNegativeOffset():
    """
    Rewrap one video stream with a negative offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testRewrapVideoNegativeOffset.mov"
    offset = -5.5

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "", offset )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    # check output duration
    assert_equals( src_videoStream.getDuration() + offset, dst_videoStream.getDuration() )
    assert_equals( src_videoStream.getNbFrames() + ( offset * dst_videoStream.getFps() ), dst_videoStream.getNbFrames() )


# The output audio stream has not the correct number of samples.
@nottest
def testMultipleOffsetFromSameInputFile():
    """
    Process multiple streams with different offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testMultipleOffsetFromSameInputFile.mov"
    offset_1 = 10
    offset_2 = 3

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "", offset_1 )
    transcoder.add( inputFileName, 1, "", offset_2 )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]
    src_audioStream = src_properties.getAudioProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]
    dst_audioStream = dst_properties.getAudioProperties()[0]

    # check output duration
    assert_equals( src_videoStream.getDuration() + offset_1, dst_videoStream.getDuration() )
    assert_equals( src_videoStream.getNbFrames() + ( offset_1 * dst_videoStream.getFps() ), dst_videoStream.getNbFrames() )
    assert_equals( src_audioStream.getDuration() + offset_1, dst_audioStream.getDuration() )
    assert_equals( src_audioStream.getNbSamples() + ( offset_1 * dst_audioStream.getSampleRate() * dst_audioStream.getChannels() ), dst_audioStream.getNbSamples() )


def testMultipleOffsetFromSameStream():
    """
    Process same stream several times with different offset at the beginning of the process.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testMultipleOffsetFromSameStream.mov"
    offset_1 = 2
    offset_2 = -2

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    transcoder.add( inputFileName, 0, "", offset_1 )
    transcoder.add( inputFileName, 0, "", offset_2 )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream_1 = dst_properties.getVideoProperties()[0]
    dst_videoStream_2 = dst_properties.getVideoProperties()[1]

    # check output duration
    assert_equals( src_videoStream.getDuration() + offset_1, dst_videoStream_1.getDuration() )
    assert_equals( src_videoStream.getDuration() + offset_1, dst_videoStream_2.getDuration() )
    assert_equals( src_videoStream.getNbFrames() + ( offset_1 * dst_videoStream_1.getFps() ), dst_videoStream_1.getNbFrames() )
    assert_equals( src_videoStream.getNbFrames() + ( offset_1 * dst_videoStream_2.getFps() ), dst_videoStream_2.getNbFrames() )
