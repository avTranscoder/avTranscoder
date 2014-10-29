from nose.tools import *

from mediaEngine import mediaCore
from mediaEngine import mediaIO

@raises(RuntimeError)
def testTranscodeNoStream():
	"""
	Can't process with no stream.
	"""
	outputFileName = "testTranscodeNoStream.avi"

	ouputFile = mediaIO.OutputFile( outputFileName )
	transcoder = mediaIO.Transcoder( ouputFile )

	progress = mediaCore.NoDisplayProgress()
	transcoder.process( progress )


@raises(RuntimeError)
def testRewrapDummy():
	"""
	Can't rewrap a dummy stream (no sense).
	"""
	outputFileName = "testRewrapDummy.avi"

	ouputFile = mediaIO.OutputFile( outputFileName )
	transcoder = mediaIO.Transcoder( ouputFile )

	transcoder.add( "", 0, "")
	transcoder.add( "", 0, -1, "")

	progress = mediaCore.NoDisplayProgress()
	transcoder.process( progress )

@raises(RuntimeError)
def testTranscodeDummyExistingProfileWithNoEssenceDesc():
	"""
	Can't add a dummy stream with no essence desc (for encoder).
	"""
	outputFileName = "testTranscodeDummyExistingProfileWithNoEssenceDesc.avi"

	ouputFile = mediaIO.OutputFile( outputFileName )
	transcoder = mediaIO.Transcoder( ouputFile )

	transcoder.add( "", 0, "dnxhd120" )
	transcoder.add( "", 0, -1, "dnxhd120" )

	progress = mediaCore.NoDisplayProgress()
	transcoder.process( progress )

@raises(RuntimeError)
def testTranscodeDummyNewProfileWithNoEssenceDesc():
	"""
	Can't add a dummy stream with no essence desc (for encoder).
	"""
	outputFileName = "testTranscodeDummyNewProfileWithNoEssenceDesc.avi"

	ouputFile = mediaIO.OutputFile( outputFileName )
	transcoder = mediaIO.Transcoder( ouputFile )

	newProfile = {
		mediaCore.mediaProfileIdentificator : "newAudioPreset",
		mediaCore.mediaProfileIdentificatorHuman : "New audio preset",
		mediaCore.mediaProfileType : mediaCore.mediaProfileTypeAudio,
	}
	transcoder.add( "", 0, newProfile )
	transcoder.add( "", 0, -1, newProfile )

	progress = mediaCore.NoDisplayProgress()
	transcoder.process( progress )

def testTranscodeDummyAudio():
	"""
	Process one frame with a dummy audio (profile wave24b48kmono).
	"""
	outputFileName = "testTranscodeDummyAudio.wav"

	ouputFile = mediaIO.OutputFile( outputFileName )
	transcoder = mediaIO.Transcoder( ouputFile )

	# add a dummy video stream
	audioDesc = mediaCore.AudioFrameDesc()
	audioDesc.setSampleRate( 48000 )
	audioDesc.setChannels( 1 )
	audioDesc.setFps( 25 )
	audioDesc.setSampleFormat( "s16" )

	audioCodec = mediaIO.AudioCodec( mediaIO.eCodecTypeEncoder, "pcm_s16le" )
	audioCodec.setAudioParameters( audioDesc )
	transcoder.add( "", 0, "wave24b48kmono", audioCodec )

	transcoder.init()

	ouputFile.beginWrap()
	transcoder.processFrame()
	ouputFile.endWrap()

def testTranscodeDummyVideo():
	"""
	Process one frame with a dummy video (profile dnxhd120).
	"""
	outputFileName = "testTranscodeDummyVideo.avi"

	ouputFile = mediaIO.OutputFile( outputFileName )
	transcoder = mediaIO.Transcoder( ouputFile )

	# add a dummy video stream
	imageDesc = mediaCore.VideoFrameDesc()
	imageDesc.setWidth( 1920 )
	imageDesc.setHeight( 1080 )
	imageDesc.setDar( 1, 1 )
	inputPixel = mediaCore.Pixel( "yuv422p" )
	imageDesc.setPixel( inputPixel )
	videoCodec = mediaIO.VideoCodec( mediaIO.eCodecTypeEncoder, "mpeg2video" )
	videoCodec.setImageParameters( imageDesc )
	transcoder.add( "", 0, "dnxhd120", videoCodec )

	transcoder.init()

	ouputFile.beginWrap()
	transcoder.processFrame()
	ouputFile.endWrap()
