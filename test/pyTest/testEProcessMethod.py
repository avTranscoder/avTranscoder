import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_VIDEO_AVI_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_AUDIO_MOV_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_VIDEO_AVI_FILE and "
        "AVTRANSCODER_TEST_AUDIO_MOV_FILE and "
        "AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testEProcessMethodShortest():
    """
    Process with method eProcessMethodShortest, check output duration.
    """
    inputFileName_longest = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    inputFileName_shortest = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testEProcessMethodShortest.mov"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )
    transcoder.setProcessMethod( av.eProcessMethodShortest )

    transcoder.addStream( av.InputStreamDesc(inputFileName_longest, 0) )
    transcoder.addStream( av.InputStreamDesc(inputFileName_shortest, 1) )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile_shortest = av.InputFile( inputFileName_shortest )
    src_properties_shortest = src_inputFile_shortest.getProperties()

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()

    for dst_stream_properties in dst_properties.getStreamProperties():
        assert_almost_equals( dst_stream_properties.getDuration(), src_properties_shortest.getDuration(), delta=0.05 )


def testEProcessMethodLongest():
    """
    Process with method eProcessMethodLongest, check output duration.
    """
    inputFileName_longest = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    inputFileName_shortest = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testEProcessMethodLongest.mov"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )
    transcoder.setProcessMethod( av.eProcessMethodLongest )

    transcoder.addStream( av.InputStreamDesc(inputFileName_longest, 0) )
    transcoder.addStream( av.InputStreamDesc(inputFileName_shortest, 1) )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile_longest = av.InputFile( inputFileName_longest )
    src_properties_longest = src_inputFile_longest.getProperties()

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()

    for dst_stream_properties in dst_properties.getStreamProperties():
        assert_almost_equals( dst_stream_properties.getDuration(), src_properties_longest.getDuration(), delta=0.05 )


def testEProcessMethodBasedOnStream():
    """
    Process with method testEProcessMethodBasedOnStream, check output duration.
    """
    inputFileName_first = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    inputFileName_second = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    inputFileName_third = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testEProcessMethodBasedOnStream.mov"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )
    transcoder.setProcessMethod( av.eProcessMethodBasedOnStream, 1 )

    transcoder.addStream( av.InputStreamDesc(inputFileName_first, 0) )
    transcoder.addStream( av.InputStreamDesc(inputFileName_second, 0) )
    transcoder.addStream( av.InputStreamDesc(inputFileName_third, 1) )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get src file
    src_inputFile_second = av.InputFile( inputFileName_second )
    src_properties_second = src_inputFile_second.getProperties()

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()

    for dst_stream_properties in dst_properties.getStreamProperties():
        assert_almost_equals( dst_stream_properties.getDuration(), src_properties_second.getDuration(), delta=0.05 )


def testEProcessMethodBasedOnDuration():
    """
    Process with method eProcessMethodBasedOnDuration, check output duration.
    """
    inputFileName_first = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    inputFileName_second = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    inputFileName_third = os.environ['AVTRANSCODER_TEST_AUDIO_MOV_FILE']
    outputFileName = "testEProcessMethodBasedOnDuration.mov"
    outputDuration = 10

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )
    transcoder.setProcessMethod( av.eProcessMethodBasedOnDuration, 0, outputDuration )

    transcoder.addStream( av.InputStreamDesc(inputFileName_first, 0) )
    transcoder.addStream( av.InputStreamDesc(inputFileName_second, 0) )
    transcoder.addStream( av.InputStreamDesc(inputFileName_third, 1) )

    progress = av.ConsoleProgress()
    transcoder.process( progress )

    # get dst file
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()

    for dst_stream_properties in dst_properties.getStreamProperties():
        assert_almost_equals( dst_stream_properties.getDuration(), outputDuration, delta=0.1 )

