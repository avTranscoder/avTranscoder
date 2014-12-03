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
	src_inputFile.analyse( progress )
	src_properties = src_inputFile.getProperties()
	src_audioStream = src_properties.getAudioProperties()[0]

	# get dst file of wrap
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress, av.eAnalyseLevelHeader )
	dst_properties = dst_inputFile.getProperties()
	dst_audioStream = dst_properties.getAudioProperties()[0]

	# check format
	assert_equals( src_properties.getFormatName(), dst_properties.getFormatName() )
	assert_equals( src_properties.getFormatLongName(), dst_properties.getFormatLongName() )
	assert_equals( src_properties.getStartTime(), dst_properties.getStartTime() )
	assert_equals( src_properties.getDuration(), dst_properties.getDuration() )
	deltaBitRateAudio = 10
	assert_almost_equals( src_properties.getBitRate(), dst_properties.getBitRate(), delta=deltaBitRateAudio )
	assert_equals( src_properties.getPacketSize(), dst_properties.getPacketSize() )

	# check audio stream
	assert_equals( src_audioStream.getCodecName(), dst_audioStream.getCodecName() )
	assert_equals( src_audioStream.getCodecLongName(), dst_audioStream.getCodecLongName() )
	assert_equals( src_audioStream.getSampleFormatName(), dst_audioStream.getSampleFormatName() )
	assert_equals( src_audioStream.getSampleFormatLongName(), dst_audioStream.getSampleFormatLongName() )
	# assert_equals( src_audioStream.getChannelLayout(), dst_audioStream.getChannelLayout() )  # '5.1' != '0 channels'
	assert_equals( src_audioStream.getChannelName(), dst_audioStream.getChannelName() )
	assert_equals( src_audioStream.getChannelDescription(), dst_audioStream.getChannelDescription() )
	assert_equals( src_audioStream.getStreamId(), dst_audioStream.getStreamId() )
	assert_equals( src_audioStream.getCodecId(), dst_audioStream.getCodecId() )
	assert_equals( src_audioStream.getSampleRate(), dst_audioStream.getSampleRate() )
	assert_equals( src_audioStream.getChannels(), dst_audioStream.getChannels() )
	assert_equals( src_audioStream.getBitRate(), dst_audioStream.getBitRate() )

	assert_equals( len( src_audioStream.getMetadatas() ), len( dst_audioStream.getMetadatas() ) )
	for metadata in range( 0, len( src_audioStream.getMetadatas() ) ):
		src_metadata = src_audioStream.getMetadatas()[metadata]
		dst_metadata = dst_audioStream.getMetadatas()[metadata]

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
	src_inputFile.analyse( progress )
	src_properties = src_inputFile.getProperties()	
        src_videoStream = src_properties.getVideoProperties()[0]

	# get dst file of wrap
	dst_inputFile = av.InputFile( outputFileName )
	dst_inputFile.analyse( progress )
	dst_properties = dst_inputFile.getProperties()
	dst_videoStream = dst_properties.getVideoProperties()[0]

	# check format
	assert_equals( src_properties.getFormatName(), dst_properties.getFormatName() )
	assert_equals( src_properties.getFormatLongName(), dst_properties.getFormatLongName() )
	assert_equals( src_properties.getStartTime(), dst_properties.getStartTime() )
	assert_equals( src_properties.getDuration(), dst_properties.getDuration() )
	deltaBitRateVideo = 500000
	assert_almost_equals( src_properties.getBitRate(), dst_properties.getBitRate(), delta=deltaBitRateVideo )
	assert_equals( src_properties.getPacketSize(), dst_properties.getPacketSize() )

	# check video stream
	assert_equals( src_videoStream.getCodecName(), dst_videoStream.getCodecName() )
	assert_equals( src_videoStream.getCodecLongName(), dst_videoStream.getCodecLongName() )
	assert_equals( src_videoStream.getProfileName(), dst_videoStream.getProfileName() )
	assert_equals( src_videoStream.getColorTransfert(), dst_videoStream.getColorTransfert() )
	assert_equals( src_videoStream.getColorspace(), dst_videoStream.getColorspace() )
	assert_equals( src_videoStream.getColorRange(), dst_videoStream.getColorRange() )
	assert_equals( src_videoStream.getColorPrimaries(), dst_videoStream.getColorPrimaries() )
	assert_equals( src_videoStream.getChromaSampleLocation(), dst_videoStream.getChromaSampleLocation() )
	assert_equals( src_videoStream.getFieldOrder(), dst_videoStream.getFieldOrder() )
	assert_equals( src_videoStream.getPixelName(), dst_videoStream.getPixelName() )
	assert_equals( src_videoStream.getEndianess(), dst_videoStream.getEndianess() )
	assert_equals( src_videoStream.getStartTimecodeString(), dst_videoStream.getStartTimecodeString() )

	assert_equals( src_videoStream.getTimeBase().num, dst_videoStream.getTimeBase().num )
	assert_equals( src_videoStream.getTimeBase().den, dst_videoStream.getTimeBase().den )
	assert_equals( src_videoStream.getSar().num, dst_videoStream.getSar().num )
	assert_equals( src_videoStream.getSar().den, dst_videoStream.getSar().den )
	assert_equals( src_videoStream.getDar().num, dst_videoStream.getDar().num )
	assert_equals( src_videoStream.getDar().den, dst_videoStream.getDar().den )
	
	assert_equals( src_videoStream.getStreamId(), dst_videoStream.getStreamId() )
	assert_equals( src_videoStream.getCodecId(), dst_videoStream.getCodecId() )
	assert_equals( src_videoStream.getBitRate(), dst_videoStream.getBitRate() )
	assert_equals( src_videoStream.getMaxBitRate(), dst_videoStream.getMaxBitRate() )
	assert_equals( src_videoStream.getMinBitRate(), dst_videoStream.getMinBitRate() )
	assert_equals( src_videoStream.getTicksPerFrame(), dst_videoStream.getTicksPerFrame() )
	assert_equals( src_videoStream.getWidth(), dst_videoStream.getWidth() )
	assert_equals( src_videoStream.getHeight(), dst_videoStream.getHeight() )
	assert_equals( src_videoStream.getGopSize(), dst_videoStream.getGopSize() )
	assert_equals( src_videoStream.getDtgActiveFormat(), dst_videoStream.getDtgActiveFormat() )
	assert_equals( src_videoStream.getReferencesFrames(), dst_videoStream.getReferencesFrames() )
	assert_equals( src_videoStream.getProfile(), dst_videoStream.getProfile() )
	assert_equals( src_videoStream.getLevel(), dst_videoStream.getLevel() )
	assert_equals( src_videoStream.getComponentsCount(), dst_videoStream.getComponentsCount() )
	assert_equals( src_videoStream.getChromaWidth(), dst_videoStream.getChromaWidth() )
	assert_equals( src_videoStream.getChromaHeight(), dst_videoStream.getChromaHeight() )
	assert_equals( src_videoStream.getFps(), dst_videoStream.getFps() )
	assert_equals( src_videoStream.hasBFrames(), dst_videoStream.hasBFrames() )
	assert_equals( src_videoStream.isIndexedColors(), dst_videoStream.isIndexedColors() )
	assert_equals( src_videoStream.isBitWisePacked(), dst_videoStream.isBitWisePacked() )
	assert_equals( src_videoStream.isHardwareAccelerated(), dst_videoStream.isHardwareAccelerated() )
	assert_equals( src_videoStream.isPlanar(), dst_videoStream.isPlanar() )
	assert_equals( src_videoStream.isRgbPixelData(), dst_videoStream.isRgbPixelData() )
	assert_equals( src_videoStream.isPseudoPaletted(), dst_videoStream.isPseudoPaletted() )
	assert_equals( src_videoStream.hasAlpha(), dst_videoStream.hasAlpha() )
	assert_equals( src_videoStream.isInterlaced(), dst_videoStream.isInterlaced() )
	assert_equals( src_videoStream.isTopFieldFirst(), dst_videoStream.isTopFieldFirst() )

	assert_equals( len( src_videoStream.getGopStructure() ), len( dst_videoStream.getGopStructure() ) )
	for gop in range( 0, len( src_videoStream.getGopStructure() ) ):
		src_gop = src_videoStream.getGopStructure()[gop]
		dst_gop = dst_videoStream.getGopStructure()[gop]

		assert_equals( src_gop, dst_gop )

	assert_equals( len( src_videoStream.getChannels() ), len( dst_videoStream.getChannels() ) )
	for channel in range( 0, len( src_videoStream.getChannels() ) ):
		src_channel = src_videoStream.getChannels()[channel]
		dst_channel = dst_videoStream.getChannels()[channel]

		assert_equals( src_channel.id, dst_channel.id )
		assert_equals( src_channel.chromaHeight, dst_channel.chromaHeight )
		assert_equals( src_channel.bitStep, dst_channel.bitStep )

	assert_equals( len( src_videoStream.getMetadatas() ), len( dst_videoStream.getMetadatas() ) )
	for metadata in range( 0, len( src_videoStream.getMetadatas() ) ):
		src_metadata = src_videoStream.getMetadatas()[metadata]
		dst_metadata = dst_videoStream.getMetadatas()[metadata]

		assert_equals( src_metadata, dst_metadata )

