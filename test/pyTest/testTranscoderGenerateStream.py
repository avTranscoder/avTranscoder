from nose.tools import *

from pyAvTranscoder import avtranscoder as av


@raises(RuntimeError)
def testTranscodeNoStream():
    """
    Can't process with no stream.
    """
    outputFileName = "testTranscodeNoStream.avi"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    progress = av.NoDisplayProgress()
    transcoder.process( progress )


@raises(RuntimeError)
def testGenerateVideoWithIncompleteProfile():
    """
    Can't generate a video stream without an encoding profile with:
    - codec name
    - pixel format
    - width
    - height
    """
    outputFileName = "testGenerateVideoWithIncompleteProfile.avi"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    encodingProfile = {
        av.avProfileIdentificator : "newVideoPreset",
        av.avProfileIdentificatorHuman : "New video preset",
        av.avProfileType : av.avProfileTypeVideo,
    }
    transcoder.add( encodingProfile )

    progress = av.NoDisplayProgress()
    transcoder.process( progress )


@raises(RuntimeError)
def testGenerateAudioWithIncompleteProfile():
    """
    Can't generate an audio stream without an encoding profile with:
    - codec name
    - sample format
    - sample rate
    - number of channels
    """
    outputFileName = "testGenerateAudioWithIncompleteProfile.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    encodingProfile = {
        av.avProfileIdentificator : "newAudioPreset",
        av.avProfileIdentificatorHuman : "New audio preset",
        av.avProfileType : av.avProfileTypeAudio,
    }
    transcoder.add( encodingProfile )

    progress = av.NoDisplayProgress()
    transcoder.process( progress )

def testTranscodeDummyAudio():
    """
    Process one frame with a dummy audio (profile wave24b48kmono).
    """
    outputFileName = "testTranscodeDummyAudio.wav"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    # add a dummy audio stream
    transcoder.add( "wave24b48kmono" )

    ouputFile.beginWrap()
    transcoder.processFrame()
    ouputFile.endWrap()

def testTranscodeDummyVideo():
    """
    Process one frame with a dummy video (profile dnxhd120).
    """
    outputFileName = "testTranscodeDummyVideo.avi"

    ouputFile = av.OutputFile( outputFileName )
    transcoder = av.Transcoder( ouputFile )

    # add a dummy video stream
    transcoder.add( "dnxhd120" )

    ouputFile.beginWrap()
    transcoder.processFrame()
    ouputFile.endWrap()

