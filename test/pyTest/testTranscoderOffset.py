import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_WAVE_FILE') is None or os.environ.get('AVTRANSCODER_TEST_VIDEO_FILE') is None:
	from nose.plugins.skip import SkipTest
	raise SkipTest("Need to define environment variables AVTRANSCODER_TEST_VIDEO_FILE and AVTRANSCODER_TEST_AUDIO_WAVE_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testTranscodeAudioOffset():
	"""
	Transcode one audio stream (profile wave24b48kmono) with offset at the beginning of the transcode.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_WAVE_FILE']
	outputFileName = "testTranscodeAudioOffset.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave24b48kmono", 10 )

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

        # check output duration
	assert_almost_equals( src_audioStream.getDuration() + 10, dst_audioStream.getDuration(), delta=0.1 )

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

	# get src file of transcode
	src_inputFile = av.InputFile( inputFileName )
	src_properties = src_inputFile.getProperties()
	src_videoStream = src_properties.getVideoProperties()[0]

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.getVideoProperties()[0]

        # check output duration
	assert_almost_equals( src_videoStream.getDuration() + 10, dst_videoStream.getDuration(), delta=0.1 )
