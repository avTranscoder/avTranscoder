import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testTranscodeAudioOffset():
	"""
	Transcode one audio stream (profile wave24b48kmono) with offset at the beginning of the transcode.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testTranscodeAudioOffset.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave24b48kmono", 10 )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

        # TODO: check output duration


def testTranscodeVideoOffset():
	"""
	Transcode one video stream (profile mpeg2) with offset at the beginning of the transcode.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeVideoOffset.mov"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "mpeg2", 10 )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# TODO: check output duration
