from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testSetVideoFrame():
    """
    Generate a video stream, and set its frame during process.
    """
    # create output
    outputFileName = "testSetVideoFrame.mov"
    ouputFile = av.OutputFile( outputFileName )

    # create custom profile
    encodingProfile = av.ProfileMap()
    encodingProfile[av.avProfileIdentificator] = "encodingProfile"
    encodingProfile[av.avProfileIdentificatorHuman] = "custom profile"
    encodingProfile[av.avProfileType] = av.avProfileTypeVideo
    encodingProfile[av.avProfileCodec] = "mpeg2video"
    encodingProfile[av.avProfilePixelFormat] = "yuv422p"
    encodingProfile[av.avProfileWidth] = "1920"
    encodingProfile[av.avProfileHeight] = "1080"

    # create transcoder and add a video stream
    transcoder = av.Transcoder( ouputFile )
    transcoder.addStream( encodingProfile )
    videoDecoder = transcoder.getStreamTranscoder( 0 ).getCurrentDecoder()

    # start process
    ouputFile.beginWrap()
    transcoder.preProcessCodecLatency()
    p = av.ConsoleProgress()

    # process 51 frames
    nbFrames = 255
    for i in range(0, nbFrames, 5):
        transcoder.processFrame()
        p.progress( i, nbFrames )

        # set video frame
        frame = av.VideoFrame( av.VideoFrameDesc(1920, 1080, "yuv422p") )
        frame.assign(i)
        videoDecoder.setNextFrame( frame )

    # end process
    ouputFile.endWrap()

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    progress = av.NoDisplayProgress()
    dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
    dst_properties = dst_inputFile.getProperties()
    dst_videoStream = dst_properties.getVideoProperties()[0]

    assert_equals( "mpeg2video", dst_videoStream.getCodecName() )
    assert_equals( 1920, dst_videoStream.getWidth() )
    assert_equals( 1080, dst_videoStream.getHeight() )
    assert_equals( 16, dst_videoStream.getDar().num )
    assert_equals( 9, dst_videoStream.getDar().den )


def testSetAudioFrame():
    """
    Generate a audio stream, and set its frame during process.
    """
    # create output
    outputFileName = "testSetAudioFrame.wav"
    ouputFile = av.OutputFile( outputFileName )

    # create transcoder and add a video stream
    transcoder = av.Transcoder( ouputFile )
    transcoder.addStream(  "wave24b48kmono" )
    audioDecoder = transcoder.getStreamTranscoder( 0 ).getCurrentDecoder()

    # start process
    ouputFile.beginWrap()
    transcoder.preProcessCodecLatency()
    p = av.ConsoleProgress()

    # process 51 frames
    nbFrames = 255
    for i in range(0, nbFrames):
        transcoder.processFrame()
        p.progress( i, nbFrames )

        # set video frame
        frame = av.AudioFrame( av.AudioFrameDesc(48000, 1, "s32") )
        frame.assign(i)
        audioDecoder.setNextFrame( frame )

    # end process
    ouputFile.endWrap()

    # get dst file of transcode
    dst_inputFile = av.InputFile( outputFileName )
    progress = av.NoDisplayProgress()
    dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
    dst_properties = dst_inputFile.getProperties()
    dst_audioStream = dst_properties.getAudioProperties()[0]

    assert_equals( "pcm_s24le", dst_audioStream.getCodecName() )
    assert_equals( "s32", dst_audioStream.getSampleFormatName() )
    assert_equals( "signed 32 bits", dst_audioStream.getSampleFormatLongName() )
    assert_equals( 48000, dst_audioStream.getSampleRate() )
    assert_equals( 1, dst_audioStream.getNbChannels() )
