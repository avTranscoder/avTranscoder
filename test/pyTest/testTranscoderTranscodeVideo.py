import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_VIDEO_AVI_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variable AVTRANSCODER_TEST_VIDEO_AVI_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testTranscodeDnxhd120():
    """
    Transcode one video stream (profile dnxhd120).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testTranscodeDnxhd120.mxf"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_videoStream = inputFile.getProperties().getVideoProperties()[0]
    videoStreamIndex = src_videoStream.getStreamIndex()
    transcoder.add( inputFileName, videoStreamIndex, "dnxhd120" )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    videoStat = processStat.getVideoStat(0)
    # do not test duration because the profile "dnxhd120" forces the fps to 25
    assert_equals(int(src_videoStream.getDuration() * src_videoStream.getFps()), videoStat.getNbFrames())

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_inputFile.analyse(progress, av.eAnalyseLevelFirstGop)
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    assert_equals( "dnxhd", dst_videoStream.getCodecName() )
    expectedBitRate = 120000000
    deltaBitRate = expectedBitRate * 0.05
    assert_almost_equals( expectedBitRate, dst_videoStream.getBitRate(), delta=deltaBitRate )
    assert_equals( "yuv422p", dst_videoStream.getPixelProperties().getPixelName() )
    assert_equals( 1, dst_videoStream.getGopSize() )

def testTranscodeDnxhd185():
    """
    Transcode one video stream (profile dnxhd185).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testTranscodeDnxhd185.mxf"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_videoStream = inputFile.getProperties().getVideoProperties()[0]
    videoStreamIndex = src_videoStream.getStreamIndex()
    transcoder.add( inputFileName, videoStreamIndex, "dnxhd185" )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    videoStat = processStat.getVideoStat(0)
    # do not test duration because the profile "dnxhd185" forces the fps to 25
    assert_equals(int(src_videoStream.getDuration() * src_videoStream.getFps()), videoStat.getNbFrames())

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_inputFile.analyse(progress, av.eAnalyseLevelFirstGop)
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    assert_equals( "dnxhd", dst_videoStream.getCodecName() )
    expectedBitRate = 185000000
    deltaBitRate = expectedBitRate * 0.05
    assert_almost_equals( expectedBitRate, dst_videoStream.getBitRate(), delta=deltaBitRate )
    assert_equals( "yuv422p", dst_videoStream.getPixelProperties().getPixelName() )
    assert_equals( 1, dst_videoStream.getGopSize() )  

def testTranscodeDnxhd185x():
    """
    Transcode one video stream (profile dnxhd185x).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testTranscodeDnxhd185x.mxf"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_videoStream = inputFile.getProperties().getVideoProperties()[0]
    videoStreamIndex = src_videoStream.getStreamIndex()
    transcoder.add( inputFileName, videoStreamIndex, "dnxhd185x" )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    videoStat = processStat.getVideoStat(0)
    # do not test duration because the profile "dnxhd185x" forces the fps to 25
    assert_equals(int(src_videoStream.getDuration() * src_videoStream.getFps()), videoStat.getNbFrames())

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_inputFile.analyse(progress, av.eAnalyseLevelFirstGop)
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    assert_equals( "dnxhd", dst_videoStream.getCodecName() )
    expectedBitRate = 185000000
    deltaBitRate = expectedBitRate * 0.05
    assert_almost_equals( expectedBitRate, dst_videoStream.getBitRate(), delta=deltaBitRate )
    assert_equals( "yuv422p10le", dst_videoStream.getPixelProperties().getPixelName() )
    assert_equals( 1, dst_videoStream.getGopSize() )

def testTranscodeYUV420():
    """
    Process one video stream (custom profile of encoding, with pixel format YUV420).
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testTranscodeYUV420.avi"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    # create custom profile
    customProfile = av.ProfileMap()
    customProfile[av.avProfileIdentificator] = "customProfile"
    customProfile[av.avProfileIdentificatorHuman] = "custom profile"
    customProfile[av.avProfileType] = av.avProfileTypeVideo
    customProfile[av.avProfileCodec] = "mpeg2video"
    customProfile[av.avProfilePixelFormat] = "yuv420p"

    inputFile = av.InputFile( inputFileName )
    src_videoStream = inputFile.getProperties().getVideoProperties()[0]
    videoStreamIndex = src_videoStream.getStreamIndex()
    transcoder.add( inputFileName, videoStreamIndex, customProfile )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    videoStat = processStat.getVideoStat(0)
    assert_equals(src_videoStream.getDuration(), videoStat.getDuration())
    assert_equals(int(src_videoStream.getDuration() * src_videoStream.getFps()), videoStat.getNbFrames())

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    assert_equals( "mpeg2video", dst_videoStream.getCodecName() )
    assert_equals( "yuv420p", dst_videoStream.getPixelProperties().getPixelName() )
