import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_FILE') is None:
	from nose.plugins.skip import SkipTest
	raise SkipTest("Need to define environment variable AVTRANSCODER_TEST_AUDIO_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testNbSamplesAudioRewrap():
	"""
	Rewrap one audio stream, check nb samples.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testNbSamplesAudioRewrap.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

        # get src file of rewrap
	src_inputFile = av.InputFile( inputFileName )
	src_properties = src_inputFile.getProperties()
	src_audioStream = src_properties.getAudioProperties()[0]

	# get dst file of rewrap
	dst_inputFile = av.InputFile( outputFileName )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	assert_equals( src_audioStream.getNbSamples(), dst_audioStream.getNbSamples() )

def testNbSamplesAudioTranscode():
	"""
	Transcode one audio stream (to wave24b48kmono), check nb samples.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testNbSamplesAudioTranscode.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave24b48kmono" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

        # get src file of transcode
	src_inputFile = av.InputFile( inputFileName )
	src_properties = src_inputFile.getProperties()
	src_audioStream = src_properties.getAudioProperties()[0]

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	assert_equals( src_audioStream.getNbSamples(), dst_audioStream.getNbSamples() )
