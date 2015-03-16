import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testNbFramesVideoRewrap():
	"""
	Rewrap one video stream, check nb frames.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testNbFramesVideoRewrap.mov"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

        # get src file of rewrap
	src_inputFile = av.InputFile( inputFileName )
	src_properties = src_inputFile.getProperties()
	src_videoStream = src_properties.getVideoProperties()[0]

	# get dst file of rewrap
	dst_inputFile = av.InputFile( outputFileName )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.getVideoProperties()[0]

	assert_equals( src_videoStream.getNbFrames(), dst_videoStream.getNbFrames() )

def testNbFramesVideoTranscode():
	"""
	Transcode one video stream (to h264), check nb frames.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testNbFramesVideoTranscode.mov"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "mpeg2" )

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

	assert_equals( src_videoStream.getNbFrames(), dst_videoStream.getNbFrames() )
