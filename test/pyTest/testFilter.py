import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_VIDEO_AVI_FILE') is None or os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_VIDEO_AVI_FILE and "
        "AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


@raises(RuntimeError)
def testUnknwonFilter():
    """
    Try to create an unknown filter.
    """
    unknwonFilter = av.Filter("unknwonFilter")


def testFilterAttributes():
    """
    Check the attributes of an exiting filter.
    """
    # video filter
    options = "75:52";
    name = "scale"
    scale = av.Filter(name, options)

    assert_equals(scale.getName(), name)
    assert_equals(scale.getOptions(), options)
    assert_equals(scale.getInstanceName(), name)

    # audio filter
    options = "";
    name = "volume"
    instanceName = "vol1"
    volume = av.Filter(name, options, instanceName)

    # same instance name
    assert_equals(volume.getName(), name)
    assert_equals(volume.getOptions(), options)
    assert_equals(volume.getInstanceName(), instanceName)

def testVideoTranscodeWithFilter():
    """
    A video transcode with a yadif filter.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    outputFileName = "testVideoTranscodeWithFilter.avi"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_videoStream = inputFile.getProperties().getVideoProperties()[0]

    # transcode the video stream
    videoStreamIndex = src_videoStream.getStreamIndex()
    customProfile = av.ProfileMap()
    customProfile[av.avProfileIdentificator] = "customProfile"
    customProfile[av.avProfileIdentificatorHuman] = "custom profile"
    customProfile[av.avProfileType] = av.avProfileTypeVideo
    customProfile[av.avProfileCodec] = "mpeg2video"
    customProfile[av.avProfilePixelFormat] = "yuv420p"
    transcoder.addStream( av.InputStreamDesc(inputFileName, videoStreamIndex), customProfile )

    # add yadif filter
    streamTranscoder = transcoder.getStreamTranscoder(0)
    filterGraph = streamTranscoder.getFilterGraph()
    filterGraph.addFilter("yadif")

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


def testAudioTranscodeWithFilter():
    """
    An audio transcode with a volume filter.
    """
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    outputFileName = "testAudioTranscodeWithFilter.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    inputFile = av.InputFile( inputFileName )
    src_audioStream = inputFile.getProperties().getAudioProperties()[0]

    # transcode the video stream
    audioStreamIndex = src_audioStream.getStreamIndex()
    transcoder.addStream( av.InputStreamDesc(inputFileName, audioStreamIndex), "wave24b48kmono" )

    # add volume filter (here +150% of current volume)
    streamTranscoder = transcoder.getStreamTranscoder(0)
    filterGraph = streamTranscoder.getFilterGraph()
    filterGraph.addFilter("volume", "1.5")

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
