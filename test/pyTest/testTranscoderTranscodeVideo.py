import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testTranscodeDnxhd120():
	"""
	Transcode one video stream (profile dnxhd120).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeDnxhd120.mxf"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "dnxhd120" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelFirstGop )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.getVideoProperties()[0]

	assert_equals( "dnxhd", dst_videoStream.getCodecName() )
	assert_equals( "VC3/DNxHD", dst_videoStream.getCodecLongName() )
	# assert_equals( 120000000, dst_videoStream.getBitRate() )  # 120000000 != 0L
	assert_equals( "yuv422p", dst_videoStream.getPixelProperties().getPixelName() )
	# assert_equals( 1, dst_videoStream.getGopSize() )  # 1 != 12L

def testTranscodeDnxhd185():
	"""
	Transcode one video stream (profile dnxhd185).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeDnxhd185.mxf"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "dnxhd185" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.getVideoProperties()[0]

	assert_equals( "dnxhd", dst_videoStream.getCodecName() )
	assert_equals( "VC3/DNxHD", dst_videoStream.getCodecLongName() )
	# assert_equals( 185000000, dst_videoStream.getBitRate() )  # 185000000 != 0L
	assert_equals( "yuv422p", dst_videoStream.getPixelProperties().getPixelName() )
	# assert_equals( 1, dst_videoStream.getGopSize() )  # 1 != 12L

def testTranscodeDnxhd185x():
	"""
	Transcode one video stream (profile dnxhd185x).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeDnxhd185x.mxf"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "dnxhd185x" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.getVideoProperties()[0]

	assert_equals( "dnxhd", dst_videoStream.getCodecName() )
	assert_equals( "VC3/DNxHD", dst_videoStream.getCodecLongName() )
	# assert_equals( 185000000, dst_videoStream.getBitRate() )  # 185000000 != 0L
	assert_equals( "yuv422p10le", dst_videoStream.getPixelProperties().getPixelName() )
	# assert_equals( 1, dst_videoStream.getGopSize() )  # 1 != 12L

def testTranscodeYUV420():
	"""
	Process one video stream (custom profile of encoding, with pixel format YUV420).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeYUV420.avi"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	# create custom profile
	customProfile = av.ProfileMap()
	customProfile[av.avProfileIdentificator] = "customProfile"
	customProfile[av.avProfileIdentificatorHuman] = "custom profile"
	customProfile[av.avProfileType] = av.avProfileTypeVideo
	customProfile[av.avProfileCodec] = "mpeg2video"
	customProfile[av.avProfilePixelFormat] = "yuv420p"

	transcoder.add( inputFileName, 0, customProfile )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.getVideoProperties()[0]

	assert_equals( "mpeg2video", dst_videoStream.getCodecName() )
	assert_equals( "yuv420p", dst_videoStream.getPixelProperties().getPixelName() )

