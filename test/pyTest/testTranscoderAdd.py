import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_FILE') is None:
	from nose.plugins.skip import SkipTest
	raise SkipTest("Need to define environment variable AVTRANSCODER_TEST_AUDIO_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


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

