import os

# Check if environment is setup to run the tests
if os.environ.get('AVTRANSCODER_TEST_AUDIO_FILE') is None:
	from nose.plugins.skip import SkipTest
	raise SkipTest("Need to define environment variable AVTRANSCODER_TEST_AUDIO_FILE")

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

av.preloadCodecsAndFormats()
av.Logger.setLogLevel(av.AV_LOG_QUIET)


def testTranscodeWave24b48k5_1():
	"""
	Transcode one audio stream (profile wave24b48k5_1).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testTranscodeWave24b48k5_1.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave24b48k5_1" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	assert_equals( "pcm_s24le", dst_audioStream.getCodecName() )
	assert_equals( "PCM signed 24-bit little-endian", dst_audioStream.getCodecLongName() )
	assert_equals( "s32", dst_audioStream.getSampleFormatName() )
	assert_equals( "signed 32 bits", dst_audioStream.getSampleFormatLongName() )
	assert_equals( 48000, dst_audioStream.getSampleRate() )
	# assert_equals( "1 channels", dst_audioStream.getChannelLayout() )  # '1 channels' != '0 channels'
	assert_equals( 6, dst_audioStream.getChannels() )

def testTranscodeWave24b48kstereo():
	"""
	Transcode one audio stream (profile wave24b48kstereo).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testTranscodeWave24b48kstereo.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave24b48kstereo" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	assert_equals( "pcm_s24le", dst_audioStream.getCodecName() )
	assert_equals( "PCM signed 24-bit little-endian", dst_audioStream.getCodecLongName() )
	assert_equals( "s32", dst_audioStream.getSampleFormatName() )
	assert_equals( "signed 32 bits", dst_audioStream.getSampleFormatLongName() )
	assert_equals( 48000, dst_audioStream.getSampleRate() )
	# assert_equals( "1 channels", dst_audioStream.getChannelLayout() )  # '1 channels' != '0 channels'
	assert_equals( 2, dst_audioStream.getChannels() )

def testTranscodeWave24b48kmono():
	"""
	Transcode one audio stream (profile wave24b48kmono).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testTranscodeWave24b48kmono.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave24b48kmono" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	assert_equals( "pcm_s24le", dst_audioStream.getCodecName() )
	assert_equals( "PCM signed 24-bit little-endian", dst_audioStream.getCodecLongName() )
	assert_equals( "s32", dst_audioStream.getSampleFormatName() )
	assert_equals( "signed 32 bits", dst_audioStream.getSampleFormatLongName() )
	assert_equals( 48000, dst_audioStream.getSampleRate() )
	# assert_equals( "1 channels", dst_audioStream.getChannelLayout() )  # '1 channels' != '0 channels'
	assert_equals( 1, dst_audioStream.getChannels() )

def testTranscodeWave16b48kmono():
	"""
	Transcode one audio stream (profile wave16b48kmono).
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testTranscodeWave16b48kmono.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "wave16b48kmono" )

	progress = av.ConsoleProgress()
	transcoder.process( progress )

	# get dst file of transcode
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	assert_equals( "pcm_s16le", dst_audioStream.getCodecName() )
	assert_equals( "PCM signed 16-bit little-endian", dst_audioStream.getCodecLongName() )
	assert_equals( "s16", dst_audioStream.getSampleFormatName() )
	assert_equals( "signed 16 bits", dst_audioStream.getSampleFormatLongName() )
	assert_equals( 48000, dst_audioStream.getSampleRate() )
	# assert_equals( "1 channels", dst_audioStream.getChannelLayout() )  # '1 channels' != '0 channels'
	assert_equals( 1, dst_audioStream.getChannels() )
