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
def testRewrapDummy():
	"""
	Can't rewrap a dummy stream (no sense).
	"""
	outputFileName = "testRewrapDummy.avi"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( "", 0, "")
	transcoder.add( "", 0, -1, "")

	progress = av.NoDisplayProgress()
	transcoder.process( progress )

@raises(RuntimeError)
def testTranscodeDummyExistingProfileWithNoEssenceDesc():
	"""
	Can't add a dummy stream with no essence desc (for encoder).
	"""
	outputFileName = "testTranscodeDummyExistingProfileWithNoEssenceDesc.avi"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( "", 0, "dnxhd120" )
	transcoder.add( "", 0, -1, "dnxhd120" )

	progress = av.NoDisplayProgress()
	transcoder.process( progress )

@raises(RuntimeError)
def testTranscodeDummyNewProfileWithNoEssenceDesc():
	"""
	Can't add a dummy stream with no essence desc (for encoder).
	"""
	outputFileName = "testTranscodeDummyNewProfileWithNoEssenceDesc.avi"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	newProfile = {
		av.avProfileIdentificator : "newAudioPreset",
		av.avProfileIdentificatorHuman : "New audio preset",
		av.avProfileType : av.avProfileTypeAudio,
	}
	transcoder.add( "", 0, newProfile )
	transcoder.add( "", 0, -1, newProfile )

	progress = av.NoDisplayProgress()
	transcoder.process( progress )

def testTranscodeDummyAudio():
	"""
	Process one frame with a dummy audio (profile wave24b48kmono).
	"""
	outputFileName = "testTranscodeDummyAudio.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	# add a dummy video stream
	audioDesc = av.AudioFrameDesc()
	audioDesc.setSampleRate( 48000 )
	audioDesc.setChannels( 1 )
	audioDesc.setFps( 25 )
	audioDesc.setSampleFormat( "s16" )

	audioCodec = av.AudioCodec( av.eCodecTypeEncoder, "pcm_s16le" )
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

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	# add a dummy video stream
	imageDesc = av.VideoFrameDesc()
	imageDesc.setWidth( 1920 )
	imageDesc.setHeight( 1080 )
	imageDesc.setDar( 1, 1 )
	inputPixel = av.Pixel( "yuv422p" )
	imageDesc.setPixel( inputPixel )
	videoCodec = av.VideoCodec( av.eCodecTypeEncoder, "mpeg2video" )
	videoCodec.setImageParameters( imageDesc )
	transcoder.add( "", 0, "dnxhd120", videoCodec )

	transcoder.init()

	ouputFile.beginWrap()
	transcoder.processFrame()
	ouputFile.endWrap()
