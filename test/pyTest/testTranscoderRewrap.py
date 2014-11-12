import os

from nose.tools import *

from pyAvTranscoder import avtranscoder as av

def testRewrapAudioStream():
	"""
	Rewrap one audio stream.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_AUDIO_FILE']
	outputFileName = "testRewrapAudioStream.wav"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "" )

	progress = av.NoDisplayProgress()
	transcoder.process( progress )

	# get src file of wrap
	src_inputFile = av.InputFile( inputFileName )
	src_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	src_properties = src_inputFile.getProperties()
	src_audioStream = src_properties.audioStreams[0]

	# get dst file of wrap
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.audioStreams[0]

	# check format
	assert_equals( src_properties.formatName, dst_properties.formatName )
	assert_equals( src_properties.formatLongName, dst_properties.formatLongName )
	assert_equals( src_properties.startTime, dst_properties.startTime )
	assert_equals( src_properties.duration, dst_properties.duration )
	deltaBitRateAudio = 10
	assert_almost_equals( src_properties.bitRate, dst_properties.bitRate, delta=deltaBitRateAudio )
	assert_equals( src_properties.packetSize, dst_properties.packetSize )

	# check audio stream
	assert_equals( src_audioStream.codecName, dst_audioStream.codecName )
	assert_equals( src_audioStream.codecLongName, dst_audioStream.codecLongName )
	assert_equals( src_audioStream.sampleFormatName, dst_audioStream.sampleFormatName )
	assert_equals( src_audioStream.sampleFormatLongName, dst_audioStream.sampleFormatLongName )
	# assert_equals( src_audioStream.channelLayout, dst_audioStream.channelLayout )  # '5.1' != '0 channels'
	assert_equals( src_audioStream.channelName, dst_audioStream.channelName )
	assert_equals( src_audioStream.channelDescription, dst_audioStream.channelDescription )
	assert_equals( src_audioStream.streamId, dst_audioStream.streamId )
	assert_equals( src_audioStream.codecId, dst_audioStream.codecId )
	assert_equals( src_audioStream.sampleRate, dst_audioStream.sampleRate )
	assert_equals( src_audioStream.channels, dst_audioStream.channels )
	assert_equals( src_audioStream.bit_rate, dst_audioStream.bit_rate )

	assert_equals( len( src_audioStream.metadatas ), len( dst_audioStream.metadatas ) )
	for metadata in range( 0, len( src_audioStream.metadatas ) ):
		src_metadata = src_audioStream.metadatas[metadata]
		dst_metadata = dst_audioStream.metadatas[metadata]

		assert_equals( src_metadata, dst_metadata )

def testRewrapVideoStream():
	"""
	Rewrap one video stream.
	"""
	inputFileName = os.environ['AVTRANSCODER_TEST_VIDEO_FILE']
	outputFileName = "testRewrapVideoStream.avi"

	ouputFile = av.OutputFile( outputFileName )
	transcoder = av.Transcoder( ouputFile )

	transcoder.add( inputFileName, 0, "" )

	progress = av.NoDisplayProgress()
	transcoder.process( progress )

	# get src file of wrap
	src_inputFile = av.InputFile( inputFileName )
	src_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFull )
	src_properties = src_inputFile.getProperties()
	src_videoStream = src_properties.videoStreams[0]

	# get dst file of wrap
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.InputFile.eAnalyseLevelFast )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.videoStreams[0]

	# check format
	assert_equals( src_properties.formatName, dst_properties.formatName )
	assert_equals( src_properties.formatLongName, dst_properties.formatLongName )
	assert_equals( src_properties.startTime, dst_properties.startTime )
	assert_equals( src_properties.duration, dst_properties.duration )
	deltaBitRateVideo = 500000
	assert_almost_equals( src_properties.bitRate, dst_properties.bitRate, delta=deltaBitRateVideo )
	assert_equals( src_properties.packetSize, dst_properties.packetSize )

	# check video stream
	assert_equals( src_videoStream.codecName, dst_videoStream.codecName )
	assert_equals( src_videoStream.codecLongName, dst_videoStream.codecLongName )
	assert_equals( src_videoStream.profileName, dst_videoStream.profileName )
	assert_equals( src_videoStream.colorTransfert, dst_videoStream.colorTransfert )
	assert_equals( src_videoStream.colorspace, dst_videoStream.colorspace )
	assert_equals( src_videoStream.colorRange, dst_videoStream.colorRange )
	assert_equals( src_videoStream.colorPrimaries, dst_videoStream.colorPrimaries )
	assert_equals( src_videoStream.chromaSampleLocation, dst_videoStream.chromaSampleLocation )
	assert_equals( src_videoStream.fieldOrder, dst_videoStream.fieldOrder )
	assert_equals( src_videoStream.pixelName, dst_videoStream.pixelName )
	assert_equals( src_videoStream.endianess, dst_videoStream.endianess )
	assert_equals( src_videoStream.startTimecode, dst_videoStream.startTimecode )

	assert_equals( src_videoStream.timeBase.num, dst_videoStream.timeBase.num )
	assert_equals( src_videoStream.timeBase.den, dst_videoStream.timeBase.den )
	assert_equals( src_videoStream.sar.num, dst_videoStream.sar.num )
	assert_equals( src_videoStream.sar.den, dst_videoStream.sar.den )
	assert_equals( src_videoStream.dar.num, dst_videoStream.dar.num )
	assert_equals( src_videoStream.dar.den, dst_videoStream.dar.den )
	
	assert_equals( src_videoStream.streamId, dst_videoStream.streamId )
	assert_equals( src_videoStream.codecId, dst_videoStream.codecId )
	assert_equals( src_videoStream.bitRate, dst_videoStream.bitRate )
	assert_equals( src_videoStream.maxBitRate, dst_videoStream.maxBitRate )
	assert_equals( src_videoStream.minBitRate, dst_videoStream.minBitRate )
	assert_equals( src_videoStream.ticksPerFrame, dst_videoStream.ticksPerFrame )
	assert_equals( src_videoStream.width, dst_videoStream.width )
	assert_equals( src_videoStream.height, dst_videoStream.height )
	assert_equals( src_videoStream.gopSize, dst_videoStream.gopSize )
	assert_equals( src_videoStream.dtgActiveFormat, dst_videoStream.dtgActiveFormat )
	assert_equals( src_videoStream.referencesFrames, dst_videoStream.referencesFrames )
	assert_equals( src_videoStream.profile, dst_videoStream.profile )
	assert_equals( src_videoStream.level, dst_videoStream.level )
	assert_equals( src_videoStream.componentsCount, dst_videoStream.componentsCount )
	assert_equals( src_videoStream.chromaWidth, dst_videoStream.chromaWidth )
	assert_equals( src_videoStream.chromaHeight, dst_videoStream.chromaHeight )
	assert_equals( src_videoStream.fps, dst_videoStream.fps )
	assert_equals( src_videoStream.hasBFrames, dst_videoStream.hasBFrames )
	assert_equals( src_videoStream.indexedColors, dst_videoStream.indexedColors )
	assert_equals( src_videoStream.bitWisePacked, dst_videoStream.bitWisePacked )
	assert_equals( src_videoStream.hardwareAcceleration, dst_videoStream.hardwareAcceleration )
	assert_equals( src_videoStream.notFirstPlane, dst_videoStream.notFirstPlane )
	assert_equals( src_videoStream.rgbPixelData, dst_videoStream.rgbPixelData )
	assert_equals( src_videoStream.pseudoPaletted, dst_videoStream.pseudoPaletted )
	assert_equals( src_videoStream.asAlpha, dst_videoStream.asAlpha )
	assert_equals( src_videoStream.isInterlaced, dst_videoStream.isInterlaced )
	assert_equals( src_videoStream.topFieldFirst, dst_videoStream.topFieldFirst )

	assert_equals( len( src_videoStream.gopStructure ), len( dst_videoStream.gopStructure ) )
	for gop in range( 0, len( src_videoStream.gopStructure ) ):
		src_gop = src_videoStream.gopStructure[gop]
		dst_gop = dst_videoStream.gopStructure[gop]

		assert_equals( src_gop, dst_gop )

	assert_equals( len( src_videoStream.channels ), len( dst_videoStream.channels ) )
	for channel in range( 0, len( src_videoStream.channels ) ):
		src_channel = src_videoStream.channels[channel]
		dst_channel = dst_videoStream.channels[channel]

		assert_equals( src_channel.id, dst_channel.id )
		assert_equals( src_channel.chromaHeight, dst_channel.chromaHeight )
		assert_equals( src_channel.bitStep, dst_channel.bitStep )

	assert_equals( len( src_videoStream.metadatas ), len( dst_videoStream.metadatas ) )
	for metadata in range( 0, len( src_videoStream.metadatas ) ):
		src_metadata = src_videoStream.metadatas[metadata]
		dst_metadata = dst_videoStream.metadatas[metadata]

		assert_equals( src_metadata, dst_metadata )
