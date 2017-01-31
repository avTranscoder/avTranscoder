import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_VIDEO_AVI_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_VIDEO_MOV_FILE') is None or \
    os.environ.get('AVTRANSCODER_TEST_VIDEO_RAW_FILE') is None:
    from nose.plugins.skip import SkipTest
    raise SkipTest("Need to define environment variables "
        "AVTRANSCODER_TEST_AUDIO_WAVE_FILE and "
        "AVTRANSCODER_TEST_VIDEO_AVI_FILE and "
        "AVTRANSCODER_TEST_VIDEO_MOV_FILE and "
        "AVTRANSCODER_TEST_VIDEO_RAW_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def checkFormat(src_properties, dst_properties):
    """
    Check the values of the given format headers.
    """
    assert_equals( src_properties.getFormatName(), dst_properties.getFormatName() )
    assert_equals( src_properties.getStartTime(), dst_properties.getStartTime() )
    assert_equals( src_properties.getDuration(), dst_properties.getDuration() )
    assert_greater_equal( src_properties.getBitRate(), dst_properties.getBitRate() )
    assert_equals( src_properties.getPacketSize(), dst_properties.getPacketSize() )


def checkStream(src_stream, dst_stream):
    """
    Check the values of the given streams properties.
    """
    src_propertiesMap = src_stream.asMap()
    dst_propertiesMap = dst_stream.asMap()
    for key in dst_propertiesMap:
        # skip metadata of stream
        if key in [x[0] for x in src_stream.getMetadatas()]:
            continue
        # skip streamId and streamIndex (could be different from input)
        if key == 'streamId' or key == 'streamIndex':
            continue
        # skip timeBase (could be different from input)
        if key == 'timeBase':
            continue
        assert_equals( src_propertiesMap[key], dst_propertiesMap[key] )

def checkVideoStat(src_videoStream, dst_videoStat):
    """
    Check the values of the video process stats returned after a process.
    @note Because the process does not include a profile with "processStat", output quality and PSNR are not set.
    """
    assert_equals(dst_videoStat.getDuration(), src_videoStream.getDuration())
    assert_equals(dst_videoStat.getNbFrames(), int(src_videoStream.getDuration() * src_videoStream.getFps()))
    assert_equals(dst_videoStat.getQuality(), 0)
    assert_equals(dst_videoStat.getPSNR(), 0)


def testRewrapAudioStream():
    """
    Rewrap one audio stream.
    """
    # get src file of wrap
    inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_audioStream = src_properties.getAudioProperties()[0]

    formatList = src_properties.getFormatName().split(",")
    outputFileName = "testRewrapAudioStream." + formatList[0]
    ouputFile = av.OutputFile( outputFileName )

    transcoder = av.Transcoder( ouputFile )
    transcoder.addStream( av.InputStreamDesc(inputFileName) )
    processStat = transcoder.process()

    # check process stat returned
    audioStat = processStat.getAudioStat(0)
    assert_equals(src_audioStream.getDuration(), audioStat.getDuration())

    # get dst file of wrap
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    # check format
    checkFormat(src_properties, dst_properties)

    # check audio properties
    checkStream(src_audioStream, dst_audioStream)


def testRewrapAVIVideoStream():
    """
    Rewrap one video stream from avi.
    """
    # get src file of wrap
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_AVI_FILE']
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]

    formatList = src_properties.getFormatName().split(",")
    outputFileName = "testRewrapAVIVideoStream." + formatList[0]
    ouputFile = av.OutputFile( outputFileName )

    transcoder = av.Transcoder( ouputFile )
    transcoder.addStream( av.InputStreamDesc(inputFileName) )
    processStat = transcoder.process()

    # check process stat returned
    checkVideoStat(src_videoStream, processStat.getVideoStat(0))

    # get dst file of wrap
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    # check format
    checkFormat(src_properties, dst_properties)

    # check video properties
    checkStream(src_videoStream, dst_videoStream)


def testRewrapMOVVideoStream():
    """
    Rewrap one video stream from mov.
    """
    # get src file of wrap
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_MOV_FILE']
    src_inputFile = av.InputFile( inputFileName )
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]

    formatList = src_properties.getFormatName().split(",")
    outputFileName = "testRewrapMOVVideoStream." + formatList[0]
    ouputFile = av.OutputFile( outputFileName )

    transcoder = av.Transcoder( ouputFile )
    transcoder.addStream( av.InputStreamDesc(inputFileName, 1) )
    processStat = transcoder.process()

    # check process stat returned
    checkVideoStat(src_videoStream, processStat.getVideoStat(0))

    # get dst file of wrap
    dst_inputFile = av.InputFile( outputFileName )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    # check format
    checkFormat(src_properties, dst_properties)

    # check video properties
    checkStream(src_videoStream, dst_videoStream)


def testRewrapRawVideoStream():
    """
    Rewrap one raw video stream (no format).
    """
    # get src file of wrap
    inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_RAW_FILE']
    src_inputFile = av.InputFile(inputFileName)
    src_inputFile.analyse(av.NoDisplayProgress(), av.eAnalyseLevelFirstGop)
    src_properties = src_inputFile.getProperties()
    src_videoStream = src_properties.getVideoProperties()[0]

    formatList = src_properties.getFormatName().split(",")
    outputFileName = "testRewrapRawVideoStream." + formatList[0]
    ouputFile = av.OutputFile(outputFileName)

    transcoder = av.Transcoder(ouputFile)
    transcoder.addStream( av.InputStreamDesc(inputFileName) )
    processStat = transcoder.process()

    # check process stat returned
    checkVideoStat(src_videoStream, processStat.getVideoStat(0))

    # get dst file of wrap
    dst_inputFile = av.InputFile(outputFileName)
    dst_inputFile.analyse(av.NoDisplayProgress(), av.eAnalyseLevelFirstGop)
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    # check format
    checkFormat(src_properties, dst_properties)

    # check video properties
    checkStream(src_videoStream, dst_videoStream)
