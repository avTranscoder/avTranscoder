import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av


def testTranscodeWave24b48kmono():
	"""
	Transcode one audio stream (profile wave24b48kmono).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testTranscodeWave24b48kmono.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave24b48kmono" )

	transcoder.init()
	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.audioStreams[0]

	assert_equals( "pcm_s24le", dst_audioStream.codecName )
	assert_equals( "PCM signed 24-bit little-endian", dst_audioStream.codecLongName )
	assert_equals( "signed 32 bits", dst_audioStream.sampleFormat )
	assert_equals( 48000, dst_audioStream.sampleRate )
	assert_equals( "1 channels", dst_audioStream.channelLayout )
	assert_equals( 1, dst_audioStream.channels )

def testTranscodeWave16b48kmono():
	"""
	Transcode one audio stream (profile wave16b48kmono).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testTranscodeWave16b48kmono.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave16b48kmono" )

	transcoder.init()
	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.audioStreams[0]

	assert_equals( "pcm_s16le", dst_audioStream.codecName )
	assert_equals( "PCM signed 16-bit little-endian", dst_audioStream.codecLongName )
	assert_equals( "signed 16 bits", dst_audioStream.sampleFormat )
	assert_equals( 48000, dst_audioStream.sampleRate )
	assert_equals( "1 channels", dst_audioStream.channelLayout )
	assert_equals( 1, dst_audioStream.channels )

def testTranscodeDnxhd120():
	"""
	Transcode one video stream (profile dnxhd120).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeDnxhd120.mxf"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "dnxhd120" )

	transcoder.init()
	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.videoStreams[0]

	assert_equals( "dnxhd", dst_videoStream.codecName )
	assert_equals( "VC3/DNxHD", dst_videoStream.codecLongName )
	assert_equals( 120000000, dst_videoStream.bitRate )
	assert_equals( "yuv422p", dst_videoStream.pixelName )
	assert_equals( 1, dst_videoStream.gopSize )
	assert_equals( 25, dst_videoStream.fps )

def testTranscodeDnxhd185():
	"""
	Transcode one video stream (profile dnxhd185).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeDnxhd185.mxf"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "dnxhd185" )

	transcoder.init()
	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.videoStreams[0]

	assert_equals( "dnxhd", dst_videoStream.codecName )
	assert_equals( "VC3/DNxHD", dst_videoStream.codecLongName )
	assert_equals( 185000000, dst_videoStream.bitRate )
	assert_equals( "yuv422p", dst_videoStream.pixelName )
	assert_equals( 1, dst_videoStream.gopSize )
	assert_equals( 25, dst_videoStream.fps )

def testTranscodeDnxhd185x():
	"""
	Transcode one video stream (profile dnxhd185x).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeDnxhd185x.mxf"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "dnxhd185x" )

	transcoder.init()
	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.videoStreams[0]

	assert_equals( "dnxhd", dst_videoStream.codecName )
	assert_equals( "VC3/DNxHD", dst_videoStream.codecLongName )
	assert_equals( 185000000, dst_videoStream.bitRate )
	assert_equals( "yuv422p10", dst_videoStream.pixelName )
	assert_equals( 1, dst_videoStream.gopSize )
	assert_equals( 25, dst_videoStream.fps )

def testTranscodeXdcamhd422():
	"""
	Transcode one video stream (profile xdcamhd422).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testTranscodeXdcamhd422.mxf"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "xdcamhd422" )

	transcoder.init()
	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.videoStreams[0]

	assert_equals( "mpeg2video", dst_videoStream.codecName )
	assert_equals( "MPEG-2 video", dst_videoStream.codecLongName )
	assert_equals( 0, dst_videoStream.profile )
	assert_equals( 2, dst_videoStream.level )
	assert_equals( 12, dst_videoStream.gopSize )
	assert_equals( True, dst_videoStream.hasBFrames )
	assert_equals( 10, dst_videoStream.dtgActiveFormat )
	assert_equals( 25, dst_videoStream.fps )
	assert_equals( 1, dst_videoStream.colorspace )
	assert_equals( 1, dst_videoStream.colorTransfert )
	assert_equals( 1, dst_videoStream.colorPrimaries )
	assert_equals( 1, dst_videoStream.colorRange )
	assert_equals( 900000, dst_videoStream.startTimecode )
	assert_equals( 50000, dst_videoStream.bitRate )
	assert_equals( 50000, dst_videoStream.maxBitRate )
	assert_equals( 50000, dst_videoStream.minBitRate )
	assert_equals( "tt", dst_videoStream.fieldOrder )
