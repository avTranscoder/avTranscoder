import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_VIDEO_AVI_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_VIDEO_AVI_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testProcessWithStatistics():
    """
    Process one video stream with a custom profile of encoding to activate statistics.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testProcessWithStatistics.mov"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    # create custom profile
    customProfile = av.ProfileMap()
    customProfile[av.avProfileIdentificator] = "customProfile"
    customProfile[av.avProfileIdentificatorHuman] = "custom profile"
    customProfile[av.avProfileType] = av.avProfileTypeVideo
    customProfile[av.avProfileCodec] = "mpeg2video"
    customProfile[av.avProfileProcessStat] = "processStat"

    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]
    videoStreamIndex = src_videoStream.getStreamIndex()
    transcoder.add( av.InputStreamDesc(inputFileName, videoStreamIndex), customProfile )

    progress = av.ConsoleProgress()
    processStat = transcoder.process( progress )

    # check process stat returned
    videoStat = processStat.getVideoStat(0)
    assert_equals(videoStat.getDuration(), src_videoStream.getDuration())
    assert_equals(videoStat.getNbFrames(), int(src_videoStream.getDuration() * src_videoStream.getFps()))
    assert_not_equals(videoStat.getQuality(), 0)
    assert_not_equals(videoStat.getPSNR(), 0)
