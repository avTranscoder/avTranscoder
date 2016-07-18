import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_IMAGE_PNG_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_IMAGE_JPG_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
            "AVTRANSCODER_TEST_IMAGE_PNG_FILE and "
            "AVTRANSCODER_TEST_IMAGE_JPG_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testTranscodePngToMjpeg():
    """
    Transcode one image (to mjpeg).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_IMAGE_PNG_FILE']
    outputFileName = "testTranscodePngToMjpeg.jpg"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_videoStream = inputFile.getProperties().getVideoProperties()[0]
    videoStreamIndex = src_videoStream.getStreamIndex()
    transcoder.addStream( av.InputStreamDesc(inputFileName, videoStreamIndex), "mjpeg" )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    videoStat = processStat.getVideoStat(0)
    assert_equals(1, videoStat.getNbFrames())

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    assert_equals( "mjpeg", dst_videoStream.getCodecName() )
    assert_equals( "yuvj420p", dst_videoStream.getPixelProperties().getPixelName() )


def testTranscodeJpgToMjpeg():
    """
    Transcode one image (to mjpeg).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_IMAGE_JPG_FILE']
    outputFileName = "testTranscodeJpgToMjpeg.jpg"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_videoStream = inputFile.getProperties().getVideoProperties()[0]
    videoStreamIndex = src_videoStream.getStreamIndex()
    transcoder.addStream( av.InputStreamDesc(inputFileName, videoStreamIndex), "mjpeg" )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    videoStat = processStat.getVideoStat(0)
    assert_equals(1, videoStat.getNbFrames())

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    assert_equals( "mjpeg", dst_videoStream.getCodecName() )
    assert_equals( "yuvj420p", dst_videoStream.getPixelProperties().getPixelName() )
