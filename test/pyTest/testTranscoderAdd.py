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
	inputFile.activateStream( inputIndex )

	# output
	outputFileName = "testAddStreamTranscoder.avi"
	ouputFile = av.OutputFile( outputFileName )

	streamTranscoder = av.StreamTranscoder( inputFile.getStream( inputIndex ), ouputFile )
	transcoder = av.Transcoder( ouputFile )
	transcoder.add( streamTranscoder)

	# process
	progress = av.NoDisplayProgress()
	transcoder.process( progress )

