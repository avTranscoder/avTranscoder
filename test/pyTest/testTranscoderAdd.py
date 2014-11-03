import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testAddStreamTranscoder():
	"""
	Add a streamTranscoder to the Transcoder, and process rewrap of an audio stream.
	"""
	# input
	inputFile = av.InputFile( os.environ['AVTRANSCODER_TEST_AUDIO_FILE'] )
	inputIndex = 0
	inputStream = inputFile.getStream( inputIndex )
	inputStream.setBufferred( True )

	# output
	outputFileName = "testAddStreamTranscoder.avi"
	ouputFile = av.OutputFile( outputFileName )

	streamTranscoder = av.StreamTranscoder( inputStream, ouputFile )
	transcoder = av.Transcoder( ouputFile )
	transcoder.add( streamTranscoder)

	# process
	progress = av.NoDisplayProgress()
	transcoder.init()
	transcoder.process( progress )

